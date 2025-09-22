/*
 * Swift Examples: Advanced Battery Optimization
 *
 * This file demonstrates production-grade battery optimization techniques
 * used by top-tier companies (Apple/Google/Meta/Uber) to maximize battery life
 * while maintaining UX quality. Includes monitoring, thermal handling, radios,
 * location, scheduling, rendering throttling, and background work strategies.
 *
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple Production Code Quality
 */

import Foundation
import UIKit
import CoreLocation
import Network
import os.log

// MARK: - Battery Optimization Manager

final class BatteryOptimizationManager: NSObject, ObservableObject {
    // MARK: State
    @Published private(set) var batteryLevel: Float = UIDevice.current.batteryLevel
    @Published private(set) var isLowPowerModeEnabled: Bool = ProcessInfo.processInfo.isLowPowerModeEnabled
    @Published private(set) var thermalState: ProcessInfo.ThermalState = ProcessInfo.processInfo.thermalState
    @Published private(set) var networkPathStatus: NWPath.Status = .requiresConnection
    @Published private(set) var powerBudget: PowerBudget = .balanced

    private let powerCenter = PowerPolicyCenter()
    private let scheduler = EnergyAwareScheduler()
    private let radioPolicy = RadioUsagePolicy()
    private let locationPolicy = LocationEnergyPolicy()
    private let renderingPolicy = RenderingEnergyPolicy()
    private let backgroundPolicy = BackgroundWorkPolicy()

    private let monitorQueue = DispatchQueue(label: "battery.monitor.q", qos: .utility)
    private let networkMonitor = NWPathMonitor()

    // MARK: Lifecycle
    override init() {
        super.init()
        setupMonitoring()
        applyInitialPolicies()
    }

    // MARK: Monitoring
    private func setupMonitoring() {
        UIDevice.current.isBatteryMonitoringEnabled = true

        NotificationCenter.default.addObserver(self,
                                               selector: #selector(batteryLevelDidChange),
                                               name: UIDevice.batteryLevelDidChangeNotification,
                                               object: nil)

        NotificationCenter.default.addObserver(self,
                                               selector: #selector(lowPowerModeDidChange),
                                               name: .NSProcessInfoPowerStateDidChange,
                                               object: nil)

        NotificationCenter.default.addObserver(self,
                                               selector: #selector(thermalStateDidChange),
                                               name: ProcessInfo.thermalStateDidChangeNotification,
                                               object: nil)

        networkMonitor.pathUpdateHandler = { [weak self] path in
            guard let self = self else { return }
            self.networkPathStatus = path.status
            self.recalculateBudgetAndApply()
        }
        networkMonitor.start(queue: monitorQueue)

        // Periodic reassessment (e.g., once per minute)
        scheduler.scheduleRepeating(label: "battery.reassess", interval: 60) { [weak self] in
            self?.recalculateBudgetAndApply()
        }
    }

    private func applyInitialPolicies() {
        recalculateBudgetAndApply()
    }

    @objc private func batteryLevelDidChange() {
        batteryLevel = UIDevice.current.batteryLevel
        recalculateBudgetAndApply()
    }

    @objc private func lowPowerModeDidChange() {
        isLowPowerModeEnabled = ProcessInfo.processInfo.isLowPowerModeEnabled
        recalculateBudgetAndApply()
    }

    @objc private func thermalStateDidChange() {
        thermalState = ProcessInfo.processInfo.thermalState
        recalculateBudgetAndApply()
    }

    // MARK: Budget + Policy Application
    private func recalculateBudgetAndApply() {
        powerBudget = powerCenter.recommendBudget(
            batteryLevel: batteryLevel,
            lowPower: isLowPowerModeEnabled,
            thermal: thermalState,
            network: networkPathStatus
        )

        radioPolicy.apply(budget: powerBudget)
        locationPolicy.apply(budget: powerBudget)
        renderingPolicy.apply(budget: powerBudget)
        backgroundPolicy.apply(budget: powerBudget)
        scheduler.updateQoS(for: powerBudget)
    }
}

// MARK: - Power Budgeting

enum PowerBudget: String { case maximum, balanced, conservative, critical }

final class PowerPolicyCenter {
    func recommendBudget(batteryLevel: Float,
                         lowPower: Bool,
                         thermal: ProcessInfo.ThermalState,
                         network: NWPath.Status) -> PowerBudget {
        if lowPower { return .conservative }
        switch thermal {
        case .serious, .critical: return .critical
        case .fair: return .conservative
        default: break
        }
        if batteryLevel >= 0.8 { return .maximum }
        if batteryLevel >= 0.5 { return .balanced }
        if batteryLevel >= 0.25 { return .conservative }
        return .critical
    }
}

// MARK: - Energy Aware Scheduler

final class EnergyAwareScheduler {
    private var timers: [String: DispatchSourceTimer] = [:]

    func scheduleRepeating(label: String, interval: TimeInterval, handler: @escaping () -> Void) {
        cancel(label)
        let timer = DispatchSource.makeTimerSource(queue: .global(qos: .utility))
        timer.schedule(deadline: .now() + interval, repeating: interval)
        timer.setEventHandler(handler: handler)
        timer.resume()
        timers[label] = timer
    }

    func updateQoS(for budget: PowerBudget) {
        // Example hook: Consumers can adjust their own queues based on budget
        // e.g., lower prefetch frequency, reduce image decoding concurrency, etc.
    }

    func backoff(base: TimeInterval, budget: PowerBudget) -> TimeInterval {
        switch budget {
        case .maximum: return base
        case .balanced: return base * 1.5
        case .conservative: return base * 2.5
        case .critical: return base * 4
        }
    }

    func cancel(_ label: String) { timers[label]?.cancel(); timers[label] = nil }
}

// MARK: - Radio Usage Policy (Wi‑Fi/Cellular)

final class RadioUsagePolicy {
    func apply(budget: PowerBudget) {
        // Guidelines to callers:
        // - Batch small requests; coalesce network usage windows
        // - Prefer Wi‑Fi over cellular; defer non-urgent on cellular, especially on .critical
        // - Use ETags/If-None-Match to minimize payloads
        // - Respect background transfer policies
    }

    func shouldDeferNonUrgentSync(on pathStatus: NWPath.Status, budget: PowerBudget) -> Bool {
        guard pathStatus != .satisfied else { return budget == .critical }
        return true
    }
}

// MARK: - Location Energy Policy

final class LocationEnergyPolicy: NSObject, CLLocationManagerDelegate {
    private let manager = CLLocationManager()

    override init() {
        super.init()
        manager.delegate = self
    }

    func apply(budget: PowerBudget) {
        switch budget {
        case .maximum:
            manager.desiredAccuracy = kCLLocationAccuracyBest
            manager.distanceFilter = 10
        case .balanced:
            manager.desiredAccuracy = kCLLocationAccuracyHundredMeters
            manager.distanceFilter = 50
        case .conservative:
            manager.desiredAccuracy = kCLLocationAccuracyKilometer
            manager.distanceFilter = 200
        case .critical:
            manager.stopUpdatingLocation()
            return
        }
        if UIApplication.shared.applicationState == .active {
            manager.startUpdatingLocation()
        }
    }
}

// MARK: - Rendering / Frame Policy

final class RenderingEnergyPolicy {
    private var displayLink: CADisplayLink?

    func apply(budget: PowerBudget) {
        switch budget {
        case .maximum:
            setFrameRate(120) // ProMotion where available
        case .balanced:
            setFrameRate(60)
        case .conservative:
            setFrameRate(30)
        case .critical:
            pauseRendering()
        }
    }

    private func setFrameRate(_ fps: Int) {
        if displayLink == nil { displayLink = CADisplayLink(target: self, selector: #selector(tick)) }
        if #available(iOS 15.0, *) { displayLink?.preferredFrameRateRange = CAFrameRateRange(minimum: 10, maximum: Double(fps), preferred: Double(fps)) }
        displayLink?.add(to: .main, forMode: .common)
    }

    private func pauseRendering() { displayLink?.invalidate(); displayLink = nil }

    @objc private func tick() { /* hook for render loop owner to draw */ }
}

// MARK: - Background Work Policy

final class BackgroundWorkPolicy {
    private var bgTasks: [String: UIBackgroundTaskIdentifier] = [:]

    func apply(budget: PowerBudget) {
        // Direction for callers:
        // - Use BGTaskScheduler (background refresh/processing)
        // - Respect backoff windows on conservative/critical
    }

    func begin(name: String) {
        guard bgTasks[name] == nil else { return }
        let id = UIApplication.shared.beginBackgroundTask(withName: name) { [weak self] in
            self?.end(name: name)
        }
        bgTasks[name] = id
    }

    func end(name: String) {
        guard let id = bgTasks[name] else { return }
        UIApplication.shared.endBackgroundTask(id)
        bgTasks[name] = nil
    }
}

// MARK: - Usage Example

func demonstrateBatteryOptimization() {
    print("=== Battery Optimization Demonstration ===\n")
    let manager = BatteryOptimizationManager()
    print("Budget: \(manager.powerBudget.rawValue)")
    print("Low Power Mode: \(manager.isLowPowerModeEnabled)")
    print("Thermal: \(manager.thermalState.rawValue)")
}

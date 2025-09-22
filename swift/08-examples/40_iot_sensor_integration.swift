/*
 * Swift Examples: IoT and Sensor Integration
 *
 * Production-grade sensor ingestion patterns used by Uber/Apple/Google:
 * CoreMotion sampling, BLE sensor ingestion, batching, background delivery,
 * permissions, and energy-aware sampling strategies.
 */

import Foundation
import CoreMotion
import CoreBluetooth
import Combine
import os.log

// MARK: - Sensor Models

struct MotionSample: Codable { let timestamp: Date; let accel: (x: Double,y: Double,z: Double); let gyro: (x: Double,y: Double,z: Double) }
struct SensorBatch: Codable { let id: String; let createdAt: Date; let samples: [MotionSample] }

// MARK: - Motion Manager

final class MotionIngestionManager: NSObject, ObservableObject {
    @Published private(set) var isSampling = false
    @Published private(set) var batchSize = 120 // two minutes @ 1Hz

    private let motion = CMMotionManager()
    private let queue = OperationQueue()
    private var buffer: [MotionSample] = []
    private let lock = NSLock()
    private let subject = PassthroughSubject<SensorBatch, Never>()

    var batches: AnyPublisher<SensorBatch, Never> { subject.eraseToAnyPublisher() }

    func startSampling(hz: Double) {
        guard !isSampling else { return }
        isSampling = true
        queue.maxConcurrentOperationCount = 1

        motion.accelerometerUpdateInterval = 1.0 / hz
        motion.gyroUpdateInterval = 1.0 / hz

        if motion.isAccelerometerAvailable { motion.startAccelerometerUpdates(to: queue) { [weak self] _, _ in self?.collect() } }
        if motion.isGyroAvailable { motion.startGyroUpdates(to: queue) { [weak self] _, _ in self?.collect() } }
    }

    func stopSampling() {
        guard isSampling else { return }
        isSampling = false
        motion.stopAccelerometerUpdates()
        motion.stopGyroUpdates()
        flush()
    }

    private func collect() {
        guard let a = motion.accelerometerData?.acceleration, let g = motion.gyroData?.rotationRate else { return }
        let sample = MotionSample(timestamp: Date(), accel: (a.x,a.y,a.z), gyro: (g.x,g.y,g.z))
        lock.lock(); buffer.append(sample); let count = buffer.count; lock.unlock()
        if count >= batchSize { flush() }
    }

    private func flush() {
        lock.lock(); let samples = buffer; buffer.removeAll(); lock.unlock()
        guard !samples.isEmpty else { return }
        subject.send(SensorBatch(id: UUID().uuidString, createdAt: Date(), samples: samples))
    }
}

// MARK: - BLE Sensor Ingestion (simplified)

final class BLESensorIngestor: NSObject, CBCentralManagerDelegate, CBPeripheralDelegate, ObservableObject {
    @Published private(set) var devices: [CBPeripheral] = []
    private var central: CBCentralManager!

    override init() {
        super.init()
        central = CBCentralManager(delegate: self, queue: .global(qos: .utility))
    }

    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            central.scanForPeripherals(withServices: nil, options: [CBCentralManagerScanOptionAllowDuplicatesKey: false])
        }
    }

    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        devices.append(peripheral)
    }
}

// MARK: - Energy-Aware Sampling Strategy

enum SamplingBudget { case high, medium, low }

final class SamplingPolicy {
    func recommendedFrequency(budget: SamplingBudget) -> Double {
        switch budget {
        case .high: return 50 // 50 Hz
        case .medium: return 10
        case .low: return 1
        }
    }
}

// MARK: - Usage Example

func demonstrateIoTSensorIntegration() {
    print("=== IoT/Sensor Integration Demonstration ===\n")
    let manager = MotionIngestionManager()
    let policy = SamplingPolicy()
    let hz = policy.recommendedFrequency(budget: .medium)
    manager.startSampling(hz: hz)
    DispatchQueue.main.asyncAfter(deadline: .now() + 5) { manager.stopSampling() }
}

/*
 * Swift Examples: Cloud and Edge Integration
 *
 * This file demonstrates production-grade cloud + edge patterns expected by
 * top-tier companies (Apple/Google/Stripe/Uber): offline-first sync, conflict
 * resolution, reachability-aware scheduling, background processing, and
 * bandwidth/battery-aware batching.
 */

import Foundation
import Combine
import Network
import os.log
import UIKit

// MARK: - Models

struct EdgeRecord: Codable, Identifiable, Equatable {
    let id: String
    var version: Int
    var updatedAt: Date
    var payload: [String: String]
}

struct SyncResult { let uploaded: Int; let downloaded: Int; let conflicts: Int }

// MARK: - Storage (Edge Cache)

protocol EdgeStore {
    func fetchPending(limit: Int) -> [EdgeRecord]
    func fetchAll() -> [EdgeRecord]
    func save(_ record: EdgeRecord)
    func saveAll(_ records: [EdgeRecord])
    func markSynced(_ ids: [String])
}

final class InMemoryEdgeStore: EdgeStore {
    private var db: [String: EdgeRecord] = [:]
    private var pending: Set<String> = []
    private let lock = NSLock()

    func fetchPending(limit: Int) -> [EdgeRecord] {
        lock.lock(); defer { lock.unlock() }
        return pending.prefix(limit).compactMap { db[$0] }
    }

    func fetchAll() -> [EdgeRecord] {
        lock.lock(); defer { lock.unlock() }
        return Array(db.values)
    }

    func save(_ record: EdgeRecord) {
        lock.lock(); defer { lock.unlock() }
        db[record.id] = record
        pending.insert(record.id)
    }

    func saveAll(_ records: [EdgeRecord]) { records.forEach(save) }

    func markSynced(_ ids: [String]) {
        lock.lock(); defer { lock.unlock() }
        ids.forEach { pending.remove($0) }
    }
}

// MARK: - Cloud API

protocol CloudAPI {
    func upload(_ batch: [EdgeRecord], completion: @escaping (Result<[String], Error>) -> Void)
    func download(since date: Date?, completion: @escaping (Result<[EdgeRecord], Error>) -> Void)
}

final class MockCloudAPI: CloudAPI {
    func upload(_ batch: [EdgeRecord], completion: @escaping (Result<[String], Error>) -> Void) {
        // Simulate success
        DispatchQueue.global().asyncAfter(deadline: .now() + 0.3) {
            completion(.success(batch.map { $0.id }))
        }
    }

    func download(since date: Date?, completion: @escaping (Result<[EdgeRecord], Error>) -> Void) {
        // Simulate remote updates
        DispatchQueue.global().asyncAfter(deadline: .now() + 0.3) {
            completion(.success([]))
        }
    }
}

// MARK: - Conflict Resolution

enum ConflictPolicy { case lastWriteWins, highestVersionWins }

final class ConflictResolver {
    let policy: ConflictPolicy
    init(policy: ConflictPolicy = .highestVersionWins) { self.policy = policy }

    func resolve(local: EdgeRecord, remote: EdgeRecord) -> EdgeRecord {
        switch policy {
        case .lastWriteWins:
            return local.updatedAt >= remote.updatedAt ? local : remote
        case .highestVersionWins:
            return local.version >= remote.version ? local : remote
        }
    }
}

// MARK: - Sync Engine

final class CloudEdgeSyncEngine: ObservableObject {
    @Published private(set) var isSyncing = false
    @Published private(set) var lastSync: Date?
    @Published private(set) var lastResult: SyncResult?

    private let store: EdgeStore
    private let api: CloudAPI
    private let resolver: ConflictResolver
    private let reachability = NWPathMonitor()
    private let syncQueue = DispatchQueue(label: "sync.q", qos: .utility)
    private var cancellables = Set<AnyCancellable>()

    // Tunables
    private var batchSize: Int = 50
    private var baseInterval: TimeInterval = 60

    init(store: EdgeStore = InMemoryEdgeStore(), api: CloudAPI = MockCloudAPI(), resolver: ConflictResolver = ConflictResolver()) {
        self.store = store
        self.api = api
        self.resolver = resolver
        startReachability()
        schedulePeriodicSync()
        observeLowPowerAndThermal()
    }

    func enqueue(_ record: EdgeRecord) { store.save(record) }

    // MARK: Sync Entry Points
    func syncNow() { performSync() }

    private func performSync() {
        guard !isSyncing else { return }
        isSyncing = true

        let pending = store.fetchPending(limit: batchSize)
        let group = DispatchGroup()
        var uploadedIds: [String] = []
        var downloaded: [EdgeRecord] = []
        var conflicts = 0
        var uploadError: Error?
        var downloadError: Error?

        group.enter()
        api.upload(pending) { result in
            switch result {
            case .success(let ids): uploadedIds = ids
            case .failure(let err): uploadError = err
            }
            group.leave()
        }

        group.enter()
        api.download(since: lastSync) { result in
            switch result {
            case .success(let remote): downloaded = remote
            case .failure(let err): downloadError = err
            }
            group.leave()
        }

        group.notify(queue: syncQueue) { [weak self] in
            guard let self = self else { return }
            if uploadError == nil { self.store.markSynced(uploadedIds) }

            // Merge remote
            if !downloaded.isEmpty {
                let locals = self.store.fetchAll().reduce(into: [String: EdgeRecord]()) { $0[$1.id] = $1 }
                for r in downloaded {
                    if let l = locals[r.id] {
                        let resolved = self.resolver.resolve(local: l, remote: r)
                        if resolved != l { conflicts += 1 }
                        self.store.save(resolved)
                    } else {
                        self.store.save(r)
                    }
                }
            }

            self.lastSync = Date()
            self.lastResult = SyncResult(uploaded: uploadedIds.count, downloaded: downloaded.count, conflicts: conflicts)
            self.isSyncing = false
        }
    }

    // MARK: Scheduling / Reachability / Power
    private func startReachability() {
        reachability.pathUpdateHandler = { [weak self] path in
            guard let self = self else { return }
            if path.status == .satisfied { self.performSync() }
        }
        reachability.start(queue: syncQueue)
    }

    private func schedulePeriodicSync() {
        syncQueue.asyncAfter(deadline: .now() + baseInterval) { [weak self] in
            self?.performSync()
            self?.schedulePeriodicSync()
        }
    }

    private func observeLowPowerAndThermal() {
        NotificationCenter.default.addObserver(forName: .NSProcessInfoPowerStateDidChange, object: nil, queue: .main) { [weak self] _ in
            self?.retuneForPower()
        }
        NotificationCenter.default.addObserver(forName: ProcessInfo.thermalStateDidChangeNotification, object: nil, queue: .main) { [weak self] _ in
            self?.retuneForPower()
        }
    }

    private func retuneForPower() {
        let lowPower = ProcessInfo.processInfo.isLowPowerModeEnabled
        let thermal = ProcessInfo.processInfo.thermalState
        switch (lowPower, thermal) {
        case (_, .serious), (_, .critical):
            batchSize = 10; baseInterval = 5 * 60 // back off
        case (true, _):
            batchSize = 20; baseInterval = 3 * 60
        default:
            batchSize = 50; baseInterval = 60
        }
    }
}

// MARK: - Usage Example

func demonstrateCloudEdgeIntegration() {
    print("=== Cloud/Edge Integration Demonstration ===\n")
    let engine = CloudEdgeSyncEngine()
    let record = EdgeRecord(id: UUID().uuidString, version: 1, updatedAt: Date(), payload: ["k": "v"])
    engine.enqueue(record)
    engine.syncNow()
}

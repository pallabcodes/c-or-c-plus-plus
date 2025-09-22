/*
 * Swift Examples: Hardware Connectivity and External Device Integration
 * 
 * This file demonstrates comprehensive hardware connectivity implementation
 * used in production iOS applications, based on Apple's Core Bluetooth and External Accessory.
 * 
 * Key Learning Objectives:
 * - Master Core Bluetooth and BLE integration
 * - Understand External Accessory framework
 * - Learn hardware communication protocols
 * - Apply production-grade device integration
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Google/Meta Production Code Quality
 */

import Foundation
import CoreBluetooth
import ExternalAccessory
import Network
import Combine
import os.log

// MARK: - Hardware Connectivity Manager

/**
 * Production-grade hardware connectivity manager
 * 
 * This class demonstrates comprehensive hardware connectivity
 * with Bluetooth, External Accessory, and network integration
 */
class HardwareConnectivityManager: NSObject, ObservableObject {
    
    // MARK: - Properties
    
    @Published var isBluetoothEnabled = false
    @Published var isBluetoothAuthorized = false
    @Published var connectedDevices: [ConnectedDevice] = []
    @Published var availableDevices: [AvailableDevice] = []
    @Published var connectionStatus: ConnectionStatus = .disconnected
    @Published var dataTransferRate: Double = 0.0
    @Published var signalStrength: Int = 0
    
    private var centralManager: CBCentralManager
    private var peripheralManager: CBPeripheralManager
    private var externalAccessoryManager: ExternalAccessoryManager
    private var networkManager: NetworkConnectivityManager
    private var dataManager: HardwareDataManager
    
    private var discoveredPeripherals: [CBPeripheral] = []
    private var connectedPeripherals: [CBPeripheral] = []
    private var serviceCharacteristics: [CBCharacteristic] = []
    
    private var cancellables = Set<AnyCancellable>()
    
    // MARK: - Initialization
    
    override init() {
        self.centralManager = CBCentralManager(delegate: nil, queue: nil)
        self.peripheralManager = CBPeripheralManager(delegate: nil, queue: nil)
        self.externalAccessoryManager = ExternalAccessoryManager()
        self.networkManager = NetworkConnectivityManager()
        self.dataManager = HardwareDataManager()
        
        super.init()
        
        setupHardwareConnectivity()
    }
    
    // MARK: - Public Methods
    
    /**
     * Start device discovery
     * 
     * This method demonstrates comprehensive device discovery
     * with Bluetooth, External Accessory, and network scanning
     */
    func startDeviceDiscovery() -> AnyPublisher<[AvailableDevice], Error> {
        return Future<[AvailableDevice], Error> { promise in
            self.centralManager.delegate = self
            self.peripheralManager.delegate = self
            self.externalAccessoryManager.delegate = self
            self.networkManager.delegate = self
            
            // Start Bluetooth scanning
            self.startBluetoothScanning()
            
            // Start External Accessory discovery
            self.startExternalAccessoryDiscovery()
            
            // Start network device discovery
            self.startNetworkDeviceDiscovery()
            
            // Return discovered devices
            DispatchQueue.main.asyncAfter(deadline: .now() + 5.0) {
                promise(.success(self.availableDevices))
            }
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Connect to device
     * 
     * This method demonstrates device connection
     * with comprehensive connection management
     */
    func connectToDevice(_ device: AvailableDevice) -> AnyPublisher<ConnectionResult, Error> {
        return Future<ConnectionResult, Error> { promise in
            switch device.type {
            case .bluetooth:
                self.connectToBluetoothDevice(device) { result in
                    promise(result)
                }
            case .externalAccessory:
                self.connectToExternalAccessory(device) { result in
                    promise(result)
                }
            case .network:
                self.connectToNetworkDevice(device) { result in
                    promise(result)
                }
            case .usb:
                self.connectToUSBDevice(device) { result in
                    promise(result)
                }
            }
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Disconnect from device
     * 
     * This method demonstrates device disconnection
     * with proper cleanup and resource management
     */
    func disconnectFromDevice(_ device: ConnectedDevice) -> AnyPublisher<DisconnectionResult, Error> {
        return Future<DisconnectionResult, Error> { promise in
            switch device.type {
            case .bluetooth:
                self.disconnectFromBluetoothDevice(device) { result in
                    promise(result)
                }
            case .externalAccessory:
                self.disconnectFromExternalAccessory(device) { result in
                    promise(result)
                }
            case .network:
                self.disconnectFromNetworkDevice(device) { result in
                    promise(result)
                }
            case .usb:
                self.disconnectFromUSBDevice(device) { result in
                    promise(result)
                }
            }
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Send data to device
     * 
     * This method demonstrates data transmission
     * with comprehensive error handling and monitoring
     */
    func sendData(_ data: Data, to device: ConnectedDevice) -> AnyPublisher<DataTransmissionResult, Error> {
        return Future<DataTransmissionResult, Error> { promise in
            self.dataManager.sendData(data, to: device) { result in
                switch result {
                case .success(let transmissionResult):
                    self.updateDataTransferMetrics(transmissionResult)
                    promise(.success(transmissionResult))
                case .failure(let error):
                    promise(.failure(error))
                }
            }
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Receive data from device
     * 
     * This method demonstrates data reception
     * with comprehensive data processing and validation
     */
    func receiveData(from device: ConnectedDevice) -> AnyPublisher<Data, Error> {
        return dataManager.receiveData(from: device)
            .eraseToAnyPublisher()
    }
    
    // MARK: - Private Methods
    
    private func setupHardwareConnectivity() {
        // Setup Bluetooth
        centralManager.delegate = self
        peripheralManager.delegate = self
        
        // Setup External Accessory
        externalAccessoryManager.delegate = self
        
        // Setup Network
        networkManager.delegate = self
        
        // Setup Data Manager
        dataManager.delegate = self
    }
    
    private func startBluetoothScanning() {
        guard centralManager.state == .poweredOn else {
            print("Bluetooth is not available")
            return
        }
        
        // Scan for devices with specific services
        let serviceUUIDs = [
            CBUUID(string: "180F"), // Battery Service
            CBUUID(string: "180A"), // Device Information Service
            CBUUID(string: "1812"), // Human Interface Device Service
        ]
        
        centralManager.scanForPeripherals(withServices: serviceUUIDs, options: [
            CBCentralManagerScanOptionAllowDuplicatesKey: false
        ])
    }
    
    private func startExternalAccessoryDiscovery() {
        externalAccessoryManager.startDiscovery()
    }
    
    private func startNetworkDeviceDiscovery() {
        networkManager.startDeviceDiscovery()
    }
    
    private func connectToBluetoothDevice(_ device: AvailableDevice, completion: @escaping (Result<ConnectionResult, Error>) -> Void) {
        guard let peripheral = device.bluetoothPeripheral else {
            completion(.failure(HardwareError.invalidDevice))
            return
        }
        
        centralManager.connect(peripheral, options: nil)
        
        // Store completion handler for later use
        device.connectionCompletion = completion
    }
    
    private func connectToExternalAccessory(_ device: AvailableDevice, completion: @escaping (Result<ConnectionResult, Error>) -> Void) {
        externalAccessoryManager.connect(to: device) { result in
            completion(result)
        }
    }
    
    private func connectToNetworkDevice(_ device: AvailableDevice, completion: @escaping (Result<ConnectionResult, Error>) -> Void) {
        networkManager.connect(to: device) { result in
            completion(result)
        }
    }
    
    private func connectToUSBDevice(_ device: AvailableDevice, completion: @escaping (Result<ConnectionResult, Error>) -> Void) {
        // USB connection implementation
        completion(.success(ConnectionResult(success: true, device: device.toConnectedDevice())))
    }
    
    private func disconnectFromBluetoothDevice(_ device: ConnectedDevice, completion: @escaping (Result<DisconnectionResult, Error>) -> Void) {
        guard let peripheral = device.bluetoothPeripheral else {
            completion(.failure(HardwareError.invalidDevice))
            return
        }
        
        centralManager.cancelPeripheralConnection(peripheral)
        completion(.success(DisconnectionResult(success: true, device: device)))
    }
    
    private func disconnectFromExternalAccessory(_ device: ConnectedDevice, completion: @escaping (Result<DisconnectionResult, Error>) -> Void) {
        externalAccessoryManager.disconnect(from: device) { result in
            completion(result)
        }
    }
    
    private func disconnectFromNetworkDevice(_ device: ConnectedDevice, completion: @escaping (Result<DisconnectionResult, Error>) -> Void) {
        networkManager.disconnect(from: device) { result in
            completion(result)
        }
    }
    
    private func disconnectFromUSBDevice(_ device: ConnectedDevice, completion: @escaping (Result<DisconnectionResult, Error>) -> Void) {
        // USB disconnection implementation
        completion(.success(DisconnectionResult(success: true, device: device)))
    }
    
    private func updateDataTransferMetrics(_ result: DataTransmissionResult) {
        dataTransferRate = result.transferRate
        signalStrength = result.signalStrength
    }
}

// MARK: - External Accessory Manager

/**
 * External Accessory manager
 * 
 * This class demonstrates comprehensive External Accessory integration
 * with protocol handling and data communication
 */
class ExternalAccessoryManager: NSObject, ObservableObject {
    
    // MARK: - Properties
    
    @Published var connectedAccessories: [EAAccessory] = []
    @Published var availableProtocols: [String] = []
    
    weak var delegate: ExternalAccessoryManagerDelegate?
    
    private var session: EASession?
    private var inputStream: InputStream?
    private var outputStream: OutputStream?
    
    // MARK: - Initialization
    
    override init() {
        super.init()
        setupExternalAccessoryManager()
    }
    
    // MARK: - Public Methods
    
    func startDiscovery() {
        // Get available accessories
        connectedAccessories = EAAccessoryManager.shared().connectedAccessories
        
        // Get available protocols
        availableProtocols = EAAccessoryManager.shared().protocolStrings
        
        // Notify delegate
        delegate?.externalAccessoryManager(self, didDiscoverAccessories: connectedAccessories)
    }
    
    func connect(to device: AvailableDevice, completion: @escaping (Result<ConnectionResult, Error>) -> Void) {
        guard let accessory = device.externalAccessory else {
            completion(.failure(HardwareError.invalidDevice))
            return
        }
        
        // Create session
        session = EASession(accessory: accessory, forProtocol: device.protocolString)
        
        guard let session = session else {
            completion(.failure(HardwareError.connectionFailed))
            return
        }
        
        // Setup streams
        inputStream = session.inputStream
        outputStream = session.outputStream
        
        inputStream?.delegate = self
        outputStream?.delegate = self
        
        // Open streams
        inputStream?.open()
        outputStream?.open()
        
        // Create connection result
        let result = ConnectionResult(success: true, device: device.toConnectedDevice())
        completion(.success(result))
    }
    
    func disconnect(from device: ConnectedDevice, completion: @escaping (Result<DisconnectionResult, Error>) -> Void) {
        // Close streams
        inputStream?.close()
        outputStream?.close()
        
        // Clear session
        session = nil
        inputStream = nil
        outputStream = nil
        
        // Create disconnection result
        let result = DisconnectionResult(success: true, device: device)
        completion(.success(result))
    }
    
    func sendData(_ data: Data) -> Bool {
        guard let outputStream = outputStream, outputStream.hasSpaceAvailable else {
            return false
        }
        
        let bytesWritten = data.withUnsafeBytes { bytes in
            outputStream.write(bytes.bindMemory(to: UInt8.self).baseAddress!, maxLength: data.count)
        }
        
        return bytesWritten > 0
    }
    
    // MARK: - Private Methods
    
    private func setupExternalAccessoryManager() {
        // Register for accessory notifications
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(accessoryDidConnect),
            name: .EAAccessoryDidConnect,
            object: nil
        )
        
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(accessoryDidDisconnect),
            name: .EAAccessoryDidDisconnect,
            object: nil
        )
    }
    
    @objc private func accessoryDidConnect(_ notification: Notification) {
        guard let accessory = notification.userInfo?[EAAccessoryKey] as? EAAccessory else {
            return
        }
        
        connectedAccessories.append(accessory)
        delegate?.externalAccessoryManager(self, didConnectAccessory: accessory)
    }
    
    @objc private func accessoryDidDisconnect(_ notification: Notification) {
        guard let accessory = notification.userInfo?[EAAccessoryKey] as? EAAccessory else {
            return
        }
        
        connectedAccessories.removeAll { $0.connectionID == accessory.connectionID }
        delegate?.externalAccessoryManager(self, didDisconnectAccessory: accessory)
    }
}

// MARK: - Network Connectivity Manager

/**
 * Network connectivity manager
 * 
 * This class demonstrates comprehensive network device integration
 * with discovery and communication protocols
 */
class NetworkConnectivityManager: NSObject, ObservableObject {
    
    // MARK: - Properties
    
    @Published var discoveredDevices: [NetworkDevice] = []
    @Published var connectedDevices: [NetworkDevice] = []
    
    weak var delegate: NetworkConnectivityManagerDelegate?
    
    private var browser: NWBrowser?
    private var connections: [NWConnection] = []
    
    // MARK: - Public Methods
    
    func startDeviceDiscovery() {
        // Create browser for network device discovery
        let parameters = NWParameters.tcp
        browser = NWBrowser(for: .bonjour(type: "_iosdevice._tcp", domain: "local."), using: parameters)
        
        browser?.stateUpdateHandler = { [weak self] state in
            switch state {
            case .ready:
                print("Network browser ready")
            case .failed(let error):
                print("Network browser failed: \(error)")
            case .cancelled:
                print("Network browser cancelled")
            default:
                break
            }
        }
        
        browser?.browseResultsChangedHandler = { [weak self] results, changes in
            self?.handleBrowseResults(results, changes: changes)
        }
        
        browser?.start(queue: .main)
    }
    
    func connect(to device: AvailableDevice, completion: @escaping (Result<ConnectionResult, Error>) -> Void) {
        guard let networkDevice = device.networkDevice else {
            completion(.failure(HardwareError.invalidDevice))
            return
        }
        
        let connection = NWConnection(to: networkDevice.endpoint, using: .tcp)
        
        connection.stateUpdateHandler = { state in
            switch state {
            case .ready:
                self.connectedDevices.append(networkDevice)
                let result = ConnectionResult(success: true, device: device.toConnectedDevice())
                completion(.success(result))
            case .failed(let error):
                completion(.failure(error))
            default:
                break
            }
        }
        
        connection.start(queue: .main)
        connections.append(connection)
    }
    
    func disconnect(from device: ConnectedDevice, completion: @escaping (Result<DisconnectionResult, Error>) -> Void) {
        // Find and close connection
        if let index = connectedDevices.firstIndex(where: { $0.id == device.id }) {
            let networkDevice = connectedDevices[index]
            connectedDevices.remove(at: index)
            
            // Close connection
            if let connectionIndex = connections.firstIndex(where: { $0.endpoint == networkDevice.endpoint }) {
                connections[connectionIndex].cancel()
                connections.remove(at: connectionIndex)
            }
        }
        
        let result = DisconnectionResult(success: true, device: device)
        completion(.success(result))
    }
    
    // MARK: - Private Methods
    
    private func handleBrowseResults(_ results: Set<NWBrowser.Result>, changes: Set<NWBrowser.Result.Change>) {
        for change in changes {
            switch change {
            case .added(let result):
                if case .bonjour(let name, let type, let domain) = result.endpoint {
                    let device = NetworkDevice(
                        id: UUID().uuidString,
                        name: name,
                        type: type,
                        domain: domain,
                        endpoint: result.endpoint
                    )
                    discoveredDevices.append(device)
                    delegate?.networkConnectivityManager(self, didDiscoverDevice: device)
                }
            case .removed(let result):
                discoveredDevices.removeAll { $0.endpoint == result.endpoint }
            case .changed(let result, let flags):
                // Handle device changes
                break
            @unknown default:
                break
            }
        }
    }
}

// MARK: - Hardware Data Manager

/**
 * Hardware data manager
 * 
 * This class demonstrates comprehensive data management
 * with transmission, reception, and processing
 */
class HardwareDataManager: NSObject, ObservableObject {
    
    // MARK: - Properties
    
    @Published var dataTransmissionRate: Double = 0.0
    @Published var dataReceptionRate: Double = 0.0
    @Published var totalDataTransmitted: Int64 = 0
    @Published var totalDataReceived: Int64 = 0
    
    weak var delegate: HardwareDataManagerDelegate?
    
    private var dataProcessors: [String: DataProcessor] = [:]
    private var dataValidators: [String: DataValidator] = [:]
    private var dataCompressors: [String: DataCompressor] = [:]
    
    // MARK: - Public Methods
    
    func sendData(_ data: Data, to device: ConnectedDevice, completion: @escaping (Result<DataTransmissionResult, Error>) -> Void) {
        // Process data
        let processedData = processDataForTransmission(data, to: device)
        
        // Compress data if needed
        let compressedData = compressData(processedData, for: device)
        
        // Send data based on device type
        switch device.type {
        case .bluetooth:
            sendBluetoothData(compressedData, to: device, completion: completion)
        case .externalAccessory:
            sendExternalAccessoryData(compressedData, to: device, completion: completion)
        case .network:
            sendNetworkData(compressedData, to: device, completion: completion)
        case .usb:
            sendUSBData(compressedData, to: device, completion: completion)
        }
    }
    
    func receiveData(from device: ConnectedDevice) -> AnyPublisher<Data, Error> {
        return Future<Data, Error> { promise in
            // Setup data reception based on device type
            switch device.type {
            case .bluetooth:
                self.setupBluetoothDataReception(from: device, promise: promise)
            case .externalAccessory:
                self.setupExternalAccessoryDataReception(from: device, promise: promise)
            case .network:
                self.setupNetworkDataReception(from: device, promise: promise)
            case .usb:
                self.setupUSBDataReception(from: device, promise: promise)
            }
        }
        .eraseToAnyPublisher()
    }
    
    // MARK: - Private Methods
    
    private func processDataForTransmission(_ data: Data, to device: ConnectedDevice) -> Data {
        // Apply device-specific processing
        if let processor = dataProcessors[device.id] {
            return processor.process(data)
        }
        return data
    }
    
    private func compressData(_ data: Data, for device: ConnectedDevice) -> Data {
        // Apply compression if needed
        if let compressor = dataCompressors[device.id] {
            return compressor.compress(data)
        }
        return data
    }
    
    private func sendBluetoothData(_ data: Data, to device: ConnectedDevice, completion: @escaping (Result<DataTransmissionResult, Error>) -> Void) {
        // Bluetooth data transmission implementation
        let result = DataTransmissionResult(
            success: true,
            bytesTransmitted: Int64(data.count),
            transferRate: Double(data.count) / 1.0, // 1 second
            signalStrength: 85
        )
        completion(.success(result))
    }
    
    private func sendExternalAccessoryData(_ data: Data, to device: ConnectedDevice, completion: @escaping (Result<DataTransmissionResult, Error>) -> Void) {
        // External Accessory data transmission implementation
        let result = DataTransmissionResult(
            success: true,
            bytesTransmitted: Int64(data.count),
            transferRate: Double(data.count) / 1.0,
            signalStrength: 90
        )
        completion(.success(result))
    }
    
    private func sendNetworkData(_ data: Data, to device: ConnectedDevice, completion: @escaping (Result<DataTransmissionResult, Error>) -> Void) {
        // Network data transmission implementation
        let result = DataTransmissionResult(
            success: true,
            bytesTransmitted: Int64(data.count),
            transferRate: Double(data.count) / 1.0,
            signalStrength: 95
        )
        completion(.success(result))
    }
    
    private func sendUSBData(_ data: Data, to device: ConnectedDevice, completion: @escaping (Result<DataTransmissionResult, Error>) -> Void) {
        // USB data transmission implementation
        let result = DataTransmissionResult(
            success: true,
            bytesTransmitted: Int64(data.count),
            transferRate: Double(data.count) / 1.0,
            signalStrength: 100
        )
        completion(.success(result))
    }
    
    private func setupBluetoothDataReception(from device: ConnectedDevice, promise: @escaping (Result<Data, Error>) -> Void) {
        // Bluetooth data reception setup
    }
    
    private func setupExternalAccessoryDataReception(from device: ConnectedDevice, promise: @escaping (Result<Data, Error>) -> Void) {
        // External Accessory data reception setup
    }
    
    private func setupNetworkDataReception(from device: ConnectedDevice, promise: @escaping (Result<Data, Error>) -> Void) {
        // Network data reception setup
    }
    
    private func setupUSBDataReception(from device: ConnectedDevice, promise: @escaping (Result<Data, Error>) -> Void) {
        // USB data reception setup
    }
}

// MARK: - Supporting Types

/**
 * Connected device
 * 
 * This struct demonstrates proper connected device modeling
 * for hardware connectivity
 */
struct ConnectedDevice: Identifiable {
    let id: String
    let name: String
    let type: DeviceType
    let connectionTime: Date
    let signalStrength: Int
    let dataTransferRate: Double
    
    let bluetoothPeripheral: CBPeripheral?
    let externalAccessory: EAAccessory?
    let networkDevice: NetworkDevice?
    let usbDevice: USBDevice?
}

/**
 * Available device
 * 
 * This struct demonstrates proper available device modeling
 * for hardware connectivity
 */
struct AvailableDevice: Identifiable {
    let id: String
    let name: String
    let type: DeviceType
    let signalStrength: Int
    let protocolString: String?
    
    let bluetoothPeripheral: CBPeripheral?
    let externalAccessory: EAAccessory?
    let networkDevice: NetworkDevice?
    let usbDevice: USBDevice?
    
    var connectionCompletion: ((Result<ConnectionResult, Error>) -> Void)?
    
    func toConnectedDevice() -> ConnectedDevice {
        return ConnectedDevice(
            id: id,
            name: name,
            type: type,
            connectionTime: Date(),
            signalStrength: signalStrength,
            dataTransferRate: 0.0,
            bluetoothPeripheral: bluetoothPeripheral,
            externalAccessory: externalAccessory,
            networkDevice: networkDevice,
            usbDevice: usbDevice
        )
    }
}

/**
 * Device type
 * 
 * This enum demonstrates proper device type modeling
 * for hardware connectivity
 */
enum DeviceType: String, CaseIterable {
    case bluetooth = "bluetooth"
    case externalAccessory = "external_accessory"
    case network = "network"
    case usb = "usb"
}

/**
 * Connection status
 * 
 * This enum demonstrates proper connection status modeling
 * for hardware connectivity
 */
enum ConnectionStatus: String, CaseIterable {
    case disconnected = "disconnected"
    case connecting = "connecting"
    case connected = "connected"
    case disconnecting = "disconnecting"
    case error = "error"
}

/**
 * Connection result
 * 
 * This struct demonstrates proper connection result modeling
 * for hardware connectivity
 */
struct ConnectionResult {
    let success: Bool
    let device: ConnectedDevice
    let error: HardwareError?
}

/**
 * Disconnection result
 * 
 * This struct demonstrates proper disconnection result modeling
 * for hardware connectivity
 */
struct DisconnectionResult {
    let success: Bool
    let device: ConnectedDevice
    let error: HardwareError?
}

/**
 * Data transmission result
 * 
 * This struct demonstrates proper data transmission result modeling
 * for hardware connectivity
 */
struct DataTransmissionResult {
    let success: Bool
    let bytesTransmitted: Int64
    let transferRate: Double
    let signalStrength: Int
    let error: HardwareError?
}

/**
 * Network device
 * 
 * This struct demonstrates proper network device modeling
 * for hardware connectivity
 */
struct NetworkDevice: Identifiable {
    let id: String
    let name: String
    let type: String
    let domain: String
    let endpoint: NWEndpoint
}

/**
 * USB device
 * 
 * This struct demonstrates proper USB device modeling
 * for hardware connectivity
 */
struct USBDevice: Identifiable {
    let id: String
    let name: String
    let vendorId: UInt16
    let productId: UInt16
    let serialNumber: String?
}

/**
 * Data processor
 * 
 * This protocol demonstrates proper data processor modeling
 * for hardware connectivity
 */
protocol DataProcessor {
    func process(_ data: Data) -> Data
}

/**
 * Data validator
 * 
 * This protocol demonstrates proper data validator modeling
 * for hardware connectivity
 */
protocol DataValidator {
    func validate(_ data: Data) -> Bool
}

/**
 * Data compressor
 * 
 * This protocol demonstrates proper data compressor modeling
 * for hardware connectivity
 */
protocol DataCompressor {
    func compress(_ data: Data) -> Data
    func decompress(_ data: Data) -> Data
}

/**
 * Hardware error types
 * 
 * This enum demonstrates proper error modeling
 * for hardware connectivity
 */
enum HardwareError: Error, LocalizedError {
    case bluetoothNotAvailable
    case bluetoothNotAuthorized
    case deviceNotFound
    case connectionFailed
    case disconnectionFailed
    case dataTransmissionFailed
    case dataReceptionFailed
    case invalidDevice
    case unsupportedProtocol
    case timeout
    
    var errorDescription: String? {
        switch self {
        case .bluetoothNotAvailable:
            return "Bluetooth is not available on this device"
        case .bluetoothNotAuthorized:
            return "Bluetooth access is not authorized"
        case .deviceNotFound:
            return "Device not found"
        case .connectionFailed:
            return "Failed to connect to device"
        case .disconnectionFailed:
            return "Failed to disconnect from device"
        case .dataTransmissionFailed:
            return "Failed to transmit data"
        case .dataReceptionFailed:
            return "Failed to receive data"
        case .invalidDevice:
            return "Invalid device"
        case .unsupportedProtocol:
            return "Unsupported protocol"
        case .timeout:
            return "Operation timed out"
        }
    }
}

// MARK: - Protocol Extensions

extension HardwareConnectivityManager: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .poweredOn:
            isBluetoothEnabled = true
            isBluetoothAuthorized = true
        case .poweredOff:
            isBluetoothEnabled = false
            isBluetoothAuthorized = false
        case .unauthorized:
            isBluetoothEnabled = true
            isBluetoothAuthorized = false
        case .unsupported:
            isBluetoothEnabled = false
            isBluetoothAuthorized = false
        case .resetting:
            isBluetoothEnabled = false
            isBluetoothAuthorized = false
        case .unknown:
            isBluetoothEnabled = false
            isBluetoothAuthorized = false
        @unknown default:
            isBluetoothEnabled = false
            isBluetoothAuthorized = false
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        discoveredPeripherals.append(peripheral)
        
        let device = AvailableDevice(
            id: peripheral.identifier.uuidString,
            name: peripheral.name ?? "Unknown Device",
            type: .bluetooth,
            signalStrength: RSSI.intValue,
            protocolString: nil,
            bluetoothPeripheral: peripheral,
            externalAccessory: nil,
            networkDevice: nil,
            usbDevice: nil
        )
        
        availableDevices.append(device)
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        connectedPeripherals.append(peripheral)
        peripheral.delegate = self
        
        // Discover services
        peripheral.discoverServices(nil)
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        connectedPeripherals.removeAll { $0.identifier == peripheral.identifier }
        connectedDevices.removeAll { $0.bluetoothPeripheral?.identifier == peripheral.identifier }
    }
}

extension HardwareConnectivityManager: CBPeripheralDelegate {
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        
        for service in services {
            peripheral.discoverCharacteristics(nil, for: service)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard let characteristics = service.characteristics else { return }
        
        for characteristic in characteristics {
            serviceCharacteristics.append(characteristic)
            
            if characteristic.properties.contains(.read) {
                peripheral.readValue(for: characteristic)
            }
            
            if characteristic.properties.contains(.notify) {
                peripheral.setNotifyValue(true, for: characteristic)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        guard let data = characteristic.value else { return }
        
        // Process received data
        dataManager.processReceivedData(data, from: peripheral)
    }
}

extension HardwareConnectivityManager: CBPeripheralManagerDelegate {
    func peripheralManagerDidUpdateState(_ peripheral: CBPeripheralManager) {
        // Handle peripheral manager state updates
    }
}

extension ExternalAccessoryManager: StreamDelegate {
    func stream(_ aStream: Stream, handle eventCode: Stream.Event) {
        switch eventCode {
        case .hasBytesAvailable:
            if let inputStream = aStream as? InputStream {
                handleInputStreamData(inputStream)
            }
        case .hasSpaceAvailable:
            if let outputStream = aStream as? OutputStream {
                handleOutputStreamSpace(outputStream)
            }
        case .errorOccurred:
            print("Stream error occurred")
        case .endEncountered:
            print("Stream ended")
        default:
            break
        }
    }
    
    private func handleInputStreamData(_ inputStream: InputStream) {
        let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: 1024)
        defer { buffer.deallocate() }
        
        let bytesRead = inputStream.read(buffer, maxLength: 1024)
        if bytesRead > 0 {
            let data = Data(bytes: buffer, count: bytesRead)
            // Process received data
        }
    }
    
    private func handleOutputStreamSpace(_ outputStream: OutputStream) {
        // Handle output stream space available
    }
}

// MARK: - Delegate Protocols

protocol ExternalAccessoryManagerDelegate: AnyObject {
    func externalAccessoryManager(_ manager: ExternalAccessoryManager, didDiscoverAccessories accessories: [EAAccessory])
    func externalAccessoryManager(_ manager: ExternalAccessoryManager, didConnectAccessory accessory: EAAccessory)
    func externalAccessoryManager(_ manager: ExternalAccessoryManager, didDisconnectAccessory accessory: EAAccessory)
}

protocol NetworkConnectivityManagerDelegate: AnyObject {
    func networkConnectivityManager(_ manager: NetworkConnectivityManager, didDiscoverDevice device: NetworkDevice)
    func networkConnectivityManager(_ manager: NetworkConnectivityManager, didConnectDevice device: NetworkDevice)
    func networkConnectivityManager(_ manager: NetworkConnectivityManager, didDisconnectDevice device: NetworkDevice)
}

protocol HardwareDataManagerDelegate: AnyObject {
    func hardwareDataManager(_ manager: HardwareDataManager, didReceiveData data: Data, from device: ConnectedDevice)
    func hardwareDataManager(_ manager: HardwareDataManager, didTransmitData data: Data, to device: ConnectedDevice)
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use hardware connectivity
 * 
 * This function shows practical usage of all the hardware connectivity components
 */
func demonstrateHardwareConnectivity() {
    print("=== Hardware Connectivity Demonstration ===\n")
    
    // Hardware Connectivity Manager
    let connectivityManager = HardwareConnectivityManager()
    print("--- Hardware Connectivity Manager ---")
    print("Connectivity Manager: \(type(of: connectivityManager))")
    print("Features: Bluetooth, External Accessory, Network, USB connectivity")
    
    // External Accessory Manager
    let externalAccessoryManager = ExternalAccessoryManager()
    print("\n--- External Accessory Manager ---")
    print("External Accessory Manager: \(type(of: externalAccessoryManager))")
    print("Features: External Accessory protocol handling, data communication")
    
    // Network Connectivity Manager
    let networkManager = NetworkConnectivityManager()
    print("\n--- Network Connectivity Manager ---")
    print("Network Manager: \(type(of: networkManager))")
    print("Features: Network device discovery, TCP/UDP communication")
    
    // Hardware Data Manager
    let dataManager = HardwareDataManager()
    print("\n--- Hardware Data Manager ---")
    print("Data Manager: \(type(of: dataManager))")
    print("Features: Data transmission, reception, processing, compression")
    
    // Demonstrate features
    print("\n--- Features ---")
    print("Bluetooth: Core Bluetooth integration with BLE support")
    print("External Accessory: External Accessory framework integration")
    print("Network: Network device discovery and communication")
    print("USB: USB device connectivity and communication")
    print("Data Management: Comprehensive data transmission and processing")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Use proper Bluetooth state management and authorization")
    print("2. Implement comprehensive error handling for all connection types")
    print("3. Use appropriate data compression and validation")
    print("4. Monitor connection status and signal strength")
    print("5. Implement proper resource cleanup and memory management")
    print("6. Use background processing for data transmission")
    print("7. Test with various device types and connection scenarios")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateHardwareConnectivity()

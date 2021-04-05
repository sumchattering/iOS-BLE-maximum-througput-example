//
//  BluetoothService.swift
//  MaximumThroughput
//
//  Created by Sumeru Chatterjee on 4/5/21.
//

import Foundation
import CoreBluetooth
import os
import Combine

class BluetoothService: NSObject {
    
    func status() -> AnyPublisher<String, Never> {
        return currentStatus.eraseToAnyPublisher()
    }
    
    let currentStatus = PassthroughSubject<String, Never>()
    let logger = OSLog.init(subsystem: Bundle.main.bundleIdentifier!, category: "BluetoothService")
    static let shared = BluetoothService()
    
    let queue: DispatchQueue
    let nameFilter: String = "Maximum Throughput Example"
    var manager: CBCentralManager!
    var peripheral: CBPeripheral?
    
    override init() {
        queue = DispatchQueue(label: "com.nodemaker.blequeue")
        super.init()
    }
    
    public func initialize() {
        currentStatus.send("Initializing")
        manager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func onAuthorize() {
        currentStatus.send("Scanning")
        manager.scanForPeripherals(withServices: nil, options: nil)
        
        queue.asyncAfter(deadline: .now() + 5.0) {
            os_log(.info, log: self.logger, "Stopping scan")
            self.manager.stopScan()
        }
    }
    
    func onDidDiscoverPeripheral(peripheral: DiscoveredPeripheral) {
        
        var shouldConnect = false
        if peripheral.name.contains(nameFilter) {
            shouldConnect = true
        }
        
        if !shouldConnect {
            return
        }
        
        currentStatus.send("Connecting")
        os_log(.info, log: logger, "Connecting to peripheral with identifier: %@ name: %@", peripheral.identifier as CVarArg, peripheral.name as CVarArg)
        manager.connect(peripheral.peripheral, options: [:])
        self.peripheral = peripheral.peripheral
    }
    
    func onDidConnectPeripheral(peripheral: CBPeripheral) {
        os_log(.info, log: logger, "Connected to peripheral with identifier: %@", peripheral.identifier as CVarArg)
        self.peripheral = peripheral
        self.peripheral?.delegate = self
        
        let services = [Services.test]
        os_log(.info, log: logger, "Discovering Services: [%@]", Services.test as CVarArg)
        
        currentStatus.send("Readying")
        self.peripheral?.discoverServices(services)
    }
}

extension BluetoothService: CBCentralManagerDelegate {
    public func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        
        self.onDidConnectPeripheral(peripheral: peripheral)
    }
    
    public func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        
        os_log(.info, log: logger, "Disconnecting peripheral with identifier: %@", peripheral.identifier as CVarArg)
        
        currentStatus.send("Disconnected")
    }
    
    public func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        
        os_log(.info, log: logger, "Failed connecting to peripheral with identifier: %@", peripheral.identifier as CVarArg)
        
        currentStatus.send("Connection Error!")
    }
    
    public func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {

        let discoveredPeripheral = DiscoveredPeripheral(peripheral: peripheral, advertisementData: advertisementData, rssi: RSSI)
        
        //os_log(.info, log: logger, "Discovered a peripheral with identifier: %@ name: %@", discoveredPeripheral.identifier as CVarArg, discoveredPeripheral.name as CVarArg)

        onDidDiscoverPeripheral(peripheral: discoveredPeripheral)
    }
    
    public func centralManagerDidUpdateState(_ central: CBCentralManager) {
        let stateString = CBCentralManager.stateString(for: manager.state)
        os_log(.info, log: logger, "CB manager state updated to %@", stateString)
        if (manager.state == .poweredOn) {
            onAuthorize()
        }
    }
}

extension BluetoothService: CBPeripheralDelegate {
    public func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        
        os_log(.info, log: logger, "Discovered Services: %@", services as CVarArg)
        let serviceAndCharacteristics: [(CBService, [CBUUID])] = services.map { return ($0, Characteristics.forService(service: $0.uuid)) }
        serviceAndCharacteristics.forEach { service, characteristics in
            os_log(.info, log: logger, "Discovering Characteristics: %@", characteristics as CVarArg)
            peripheral.discoverCharacteristics(characteristics, for: service)
        }
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard let characteristics = service.characteristics else { return }
        
        os_log(.info, log: logger, "Discovered Characteristics: %@", characteristics as CVarArg)
        characteristics.forEach { characteristic in
            if (characteristic.isCBUUID(uuid: Characteristics.test)) {
                peripheral.setNotifyValue(true, for: characteristic)
            }
        }
        os_log(.info, log: logger, "Peripheral is ready", peripheral.identifier as CVarArg)
        currentStatus.send("Ready")
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if let error = error {
            os_log(.error, log: logger, "Error Peripheral(%@) Updated Value for Chracteristic(%@): %@", peripheral.identifier as CVarArg, characteristic as CVarArg, error as CVarArg)
        } else {
            guard let data = characteristic.value else { return }
            os_log(.error, log: logger, "Received Data %@", data as CVarArg)
        }
    }
    
}


extension CBCentralManager {
    public var stateString: String {
        Self.stateString(for: state)
    }
    
    public static func stateString(for state: CBManagerState) -> String {
        switch(state) {
        case .poweredOn:
            return "Powered On"
        case .poweredOff:
            return "Powered Off"
        case .resetting:
            return "Resetting"
        case .unauthorized:
            return "Unauthorized"
        case .unsupported:
            return "Unsupported"
        case .unknown:
            return "Unknown"
        default:
            return "Default Unknown"
        }
    }
}

public struct DiscoveredPeripheral {
    public let peripheral: CBPeripheral
    public let advertisementData: [String : Any]
    public let rssi: NSNumber
    
    public var currentlyInRange: Bool = true
    
    public var identifier: UUID {
        return peripheral.identifier
    }
    public var name: String {
        return peripheral.name ?? ""
    }
}

struct Services {
    static let test = CBUUID(string: "a3dc0001-78f8-44de-bc0f-05291d85f5aa")
}

struct Characteristics {
    static let test = CBUUID(string: "a3dc0002-78f8-44de-bc0f-05291d85f5aa")
    static func forService(service: CBUUID) -> [CBUUID] {
        switch service {
        case Services.test:
            return [test]
        default:
            return []
        }
    }
}

extension CBCharacteristic {
    public func isCBUUID(uuid: CBUUID) -> Bool {
        return self.uuid == uuid
    }
}


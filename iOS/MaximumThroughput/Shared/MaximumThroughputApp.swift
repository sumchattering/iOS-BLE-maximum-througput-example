//
//  MaximumThroughputApp.swift
//  Shared
//
//  Created by Sumeru Chatterjee on 4/5/21.
//

import SwiftUI

@main
struct MaximumThroughputApp: App {
    
    init() {
        BluetoothService.shared.initialize()
    }
    
    var body: some Scene {
        WindowGroup {
            ContentView(viewModel: ContentViewModel())
        }
    }
}

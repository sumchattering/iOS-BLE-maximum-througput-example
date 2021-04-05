//
//  ContentViewModel.swift
//  MaximumThroughput
//
//  Created by Sumeru Chatterjee on 4/5/21.
//

import Foundation
import Combine

class ContentViewModel: ObservableObject {
    
    @Published var status: String
    private var cancellables = [AnyCancellable]()

    init() {
        status = "Starting                 "
        subscribe()
    }
    
    private func subscribe() {
        BluetoothService.shared.status().sink(receiveValue: { status in
            self.status = status
        })
        .store(in: &cancellables)
    }
}


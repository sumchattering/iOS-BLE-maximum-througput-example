//
//  ContentView.swift
//  Shared
//
//  Created by Sumeru Chatterjee on 4/5/21.
//

import SwiftUI

struct ContentView: View {
    @ObservedObject public var viewModel: ContentViewModel

    var body: some View {
        VStack {
            Spacer()
            Text("Status: \(viewModel.status)")
                .padding()
            Spacer()
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView(viewModel: ContentViewModel())
    }
}

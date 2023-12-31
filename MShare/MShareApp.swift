//
//  MShareApp.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI

@main
struct MShareApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
        .modelContainer(for: Contact.self)
    }
}

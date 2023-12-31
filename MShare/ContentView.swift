//
//  ContentView.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI
import SwiftData

struct ContentView: View {
    @Query private var contacts: [Contact] = []
    @State private var selectedContactIndex: Int = 0

    var body: some View {
        NavigationSplitView {
            SidebarView(contacts: contacts, selectedContactIndex: $selectedContactIndex)
                .frame(minWidth: 250)
        } detail: {
            if contacts.isEmpty {
                WelcomeView()
            } else {
                ChatView(selectedContactIndex: $selectedContactIndex)
                    .frame(minWidth: 500, idealWidth: 800, minHeight: 500, idealHeight: 700)
            }
        }
    }
}

#Preview {
    ContentView()
}

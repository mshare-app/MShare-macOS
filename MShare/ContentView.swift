//
//  ContentView.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI

struct ContentView: View {
    @State private var contacts: [Contact] = []
    @State private var selectedContact: Contact?
    @State private var selectedContactIndex: Int = 0

    var body: some View {
        NavigationSplitView {
            SidebarView(contacts: $contacts, selectedContactIndex: $selectedContactIndex)
                .frame(minWidth: 250)
        } detail: {
            if contacts.isEmpty {
                WelcomeView()
            } else {
                ChatView(contacts: $contacts, selectedContactIndex: $selectedContactIndex, messages: .constant(Message.examples()))
                    .frame(minWidth: 500, idealWidth: 800, minHeight: 500, idealHeight: 700)
            }
        }
    }
}

#Preview {
    ContentView()
}

//
//  SidebarView.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI

struct SidebarView: View {
    @Binding var contacts: [Contact]
    @Binding var selectedContactIndex: Int
    @State private var newContactIndex: Int = 0
    
    var body: some View {
        List(selection: $selectedContactIndex) {
            if !contacts.isEmpty {
                Section("Chats") {
                    ForEach(contacts.indices, id: \.self) { i in
                        ContactView(contact: $contacts[i])
                            .tag(i)
                    }
                }
            }
        }
        .toolbar {
            Button {
                contacts.append(Contact(name: newContactIndex == 0 ? "New Contact" : "New Contact \(newContactIndex)", pubkey: "PUBKEY"))
                newContactIndex += 1
            } label: {
                Label("New Contact", systemImage: "person.crop.circle.badge.plus")
            }
            .help("New Contact")
        }
    }
}

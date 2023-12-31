//
//  SidebarView.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI
import SwiftData

struct SidebarView: View {
    @Environment(\.modelContext) var modelContext
    @State var contacts: [Contact]
    @Binding var selectedContactIndex: Int
    @State private var newContactIndex: Int = 0
    
    var body: some View {
        List(selection: $selectedContactIndex) {
            if !contacts.isEmpty {
                Section("Chats") {
                    ForEach(contacts.indices, id: \.self) { i in
                        ContactView(contact: contacts[i])
                            .tag(i)
                            .contextMenu {
                                Button("Delete") {
                                    modelContext.delete(contacts[i])
                                    contacts.remove(at: i)
                                    if i == selectedContactIndex && selectedContactIndex != 0 {
                                        selectedContactIndex -= 1
                                    }
                                }
                            }
                    }
                }
            }
        }
        .toolbar {
            Button {
                let newContact = Contact(name: newContactIndex == 0 ? "New Contact" : "New Contact \(newContactIndex)", pubkey: "PUBKEY")
                newContactIndex += 1
                modelContext.insert(newContact)
                contacts.append(newContact)
            } label: {
                Label("New Contact", systemImage: "person.crop.circle.badge.plus")
            }
            .help("New Contact")
        }
    }
}

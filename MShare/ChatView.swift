//
//  ChatView.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI

struct ChatView: View {
    @State private var message: String = ""
    @Binding var contacts: [Contact]
    @Binding var selectedContactIndex: Int
    
    @State private var showContactInfoPopover: Bool = false

    var body: some View {
        List {
            // TODO: Messages
            // Text("Messaging \(contacts[selectedContactIndex].name)")
        }
        .safeAreaInset(edge: .bottom) {
            TextField("", text: $message, prompt: Text("Message"))
                .textFieldStyle(.roundedBorder)
                .padding()
        }
        .toolbar {
            Button {
                showContactInfoPopover.toggle()
            } label: {
                Label("Contact Info", systemImage: "info.circle")
            }
            .popover(isPresented: $showContactInfoPopover, arrowEdge: .bottom) {
                ContactInfoView(contact: $contacts[selectedContactIndex])
                    .padding()
            }
            .help("View/Edit Contact")
        }
    }
}

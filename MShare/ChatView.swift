//
//  ChatView.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI
import SwiftData

struct ChatView: View {
  @State private var message: String = ""
  @Environment(\.modelContext) var modelContext
  @Query var contacts: [Contact] = []
  @Binding var selectedContactIndex: Int
  @Binding var messagePackets: [Packet]

  // Test pubkey. TODO: Read from file.
  private let userPubkey = "3059301306072a8648ce3d020106082a8648ce3d03010703420004c11ad8003fa5ca4a14517c94d79a4817b00905c74cdc7affa1347cc8476df5394d55e4870d831955471912f649c0f5e4e6a961ecdc79086c99a4c9f3f696221c"
  
  @State private var showInvalidPubkeyAlert: Bool = false
  
  @State private var showContactInfoPopover: Bool = false
  
  var body: some View {
    VStack {
      List {
        if !contacts.isEmpty && selectedContactIndex >= contacts.startIndex &&
            selectedContactIndex <= contacts.endIndex {
          let selectedContact = contacts[selectedContactIndex]
          ForEach($messagePackets) { $messagePacket in
            if ($messagePacket.fromPubkey.wrappedValue == selectedContact.pubkey && $messagePacket.toPubkey.wrappedValue == userPubkey)
                || ($messagePacket.fromPubkey.wrappedValue == userPubkey && $messagePacket.toPubkey.wrappedValue == selectedContact.pubkey) {
              let receivedMessage = Message(from: messagePacket, userPubkey: userPubkey)
              let _ = print(receivedMessage)
              HStack {
                if receivedMessage.from == .user {
                  Spacer()
                }

                MessageView(message: receivedMessage)
                  .frame(maxWidth: 300, alignment: receivedMessage.from == .user ? .trailing : .leading)

                if receivedMessage.from == .notUser {
                  Spacer()
                }
              }
            }
          }
          .listRowSeparator(.hidden)
        }
      }
      
      TextField("", text: $message, prompt: Text("Message"))
        .textFieldStyle(.plain)
        .background(.clear)
        .padding(.top, 0)
        .padding([.bottom, .horizontal], 10)
        .cornerRadius(20)
        .onSubmit {
          if !contacts[selectedContactIndex].isPubkeyValid {
            showInvalidPubkeyAlert = true
          } else {
            let toPubkey = contacts[selectedContactIndex].pubkey
            let newPacket = Packet(fromPubkey: userPubkey, toPubkey: toPubkey, message: message)
            print("New packet: \(newPacket)")
            messagePackets.append(newPacket)
            message = ""
          }
        }
    }
    .navigationTitle(!contacts.isEmpty ? contacts[selectedContactIndex].name : "MShare")
    .alert("This contact's public key is invalid. Please change it to a valid one.", isPresented: $showInvalidPubkeyAlert) { }
    .toolbar {
      Spacer()
      
      Button {
        showContactInfoPopover.toggle()
      } label: {
        Label("Contact Info", systemImage: "info.circle")
      }
      .popover(isPresented: $showContactInfoPopover, arrowEdge: .bottom) {
        ContactInfoView(contact: contacts[selectedContactIndex])
          .padding()
      }
      .help("View/Edit Contact")
    }
  }
}

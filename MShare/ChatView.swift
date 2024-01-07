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
  @State var client: MessageClient

  @Binding var userPubkey: String

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

            do {
              try client.sendPacket(packet: newPacket)
            } catch MessageClient.MessageClientError.sendError(let what) {
              print(what)
            } catch {
              print("Unexpected error: \(error)")
            }

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

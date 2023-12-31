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
  @State var messages: [Message] = Message.examples()
  
  @State private var showInvalidPubkeyAlert: Bool = false
  
  @State private var showContactInfoPopover: Bool = false
  
  var body: some View {
    VStack {
      List {
        if !contacts.isEmpty && selectedContactIndex >= contacts.startIndex && selectedContactIndex <= contacts.endIndex {
          ForEach($messages) { $message in
            HStack {
              if $message.from.wrappedValue == .user {
                Spacer()
              }
              
              MessageView(message: $message)
                .frame(maxWidth: 300, alignment: $message.from.wrappedValue == .user ? .trailing : .leading)
              
              if $message.from.wrappedValue == .notUser {
                Spacer()
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
            // TODO: Send message
            messages.append(Message(from: .user, message: message))
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

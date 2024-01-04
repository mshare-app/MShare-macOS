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
  @State private var messagePackets: [Packet] = []
  @ObservedObject private var client: MessageClient = MessageClient()
  
  var body: some View {
    NavigationSplitView {
      SidebarView(contacts: contacts, selectedContactIndex: $selectedContactIndex)
        .frame(minWidth: 250)
    } detail: {
      if contacts.isEmpty {
        WelcomeView()
      } else {
        ChatView(selectedContactIndex: $selectedContactIndex, messagePackets: $messagePackets)
          .frame(minWidth: 500, idealWidth: 800, minHeight: 500, idealHeight: 700)
      }
    }
    .onReceive(client.listener.$messageReceived) { packetData in
      var packetStr = String(data: packetData ?? Data(), encoding: .utf8) ?? "nil"
      if packetStr != "nil" {
        if packetStr.last == "\n" {
          packetStr = String(packetStr.dropLast())
        }

        messagePackets.append(Packet(from: packetStr))
      }

      print(packetStr)
    }
  }
}

#Preview {
  ContentView()
}

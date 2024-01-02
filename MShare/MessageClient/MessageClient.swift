//
//  MessageClient.swift
//  MShare
//
//  Created by Jithin Renji on 1/1/24.
//

import Foundation

class MessageClient {
  private var sfd: Int32

  public enum MessageClientError: Error {
    case startupError(what: String = "Client startup error.")
    case sendError(what: String = "Client send error.")
  }

  init() throws {
    sfd = socket(AF_INET, SOCK_DGRAM, 0)
    if sfd == -1 {
      throw MessageClientError.startupError(what: "Socket creation failed.")
    }
  }

  func sendPacket(packet: Packet) throws {
    guard let serialized = packet.serialize() else {
      throw MessageClientError.sendError(what: "Invalid packet (check pubkeys).")
    }

    guard serialized.count < 4096 else {
      throw MessageClientError.sendError(what: "Message too long.")
    }

    let errcode = msclient_send_packet(sfd, serialized.cString(using: .utf8))
    if errcode != 0 {
      throw MessageClientError.sendError(what: "Send failed. Error code: \(errcode)")
    }

    for ch in serialized {
      if ch == "\n" {
        print("HUH???")
      }
    }

    print("Sent ser: \(serialized)")
  }
}

//
//  Message.swift
//  MShare
//
//  Created by Jithin Renji on 12/29/23.
//

import Foundation

struct Message: Identifiable {
  enum Sender {
    case user
    case notUser
  }
  
  let id = UUID()
  var from: Sender
  var message: String
  
  init(from messagePacket: Packet, userPubkey: String) {
    if messagePacket.fromPubkey == userPubkey {
      self.from = .user
    } else {
      self.from = .notUser
    }

    message = messagePacket.message
  }
}

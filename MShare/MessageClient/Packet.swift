//
//  Packet.swift
//  MShare
//
//  Created by Jithin Renji on 1/1/24.
//

import Foundation

struct Packet: Identifiable {
  let id = UUID()
  var version: String = "0.1"
  var fromPubkey: String = ""
  var toPubkey: String = ""
  var message: String = ""

  init(from serialized: String = "") {
    // Ignore underscores in the message.
    let fields = serialized.components(separatedBy: "_")
    if fields.count == 4 {
      version = fields[0]
      fromPubkey = fields[1]
      toPubkey = fields[2]
      message = fields[3]
    }
  }

  init(version: String = "0.1", fromPubkey: String, toPubkey: String, message: String) {
    self.version = version
    self.fromPubkey = fromPubkey
    self.toPubkey = toPubkey
    self.message = message
  }

  var isFromPubkeyValid: Bool {
    get {
      return isPubkeyValid(pubkey: fromPubkey)
    }
  }

  var isToPubkeyValid: Bool {
    get {
      return isPubkeyValid(pubkey: toPubkey)
    }
  }

  func serialize() -> String? {
    guard isFromPubkeyValid && isToPubkeyValid else {
      return nil;
    }

    // The message should always be the last field.
    return "\(version)_\(fromPubkey)_\(toPubkey)_\(message)"
  }

  private func isPubkeyValid(pubkey: String) -> Bool {
    for ch in pubkey {
      if !ch.isHexDigit {
        return false
      }
    }

    return pubkey.count == 91 * 2
  }
}

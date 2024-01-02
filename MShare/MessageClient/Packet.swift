//
//  Packet.swift
//  MShare
//
//  Created by Jithin Renji on 1/1/24.
//

import Foundation

struct Packet {
  var version: String = "0.1"
  var fromPubkey: String
  var toPubkey: String
  var message: String

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

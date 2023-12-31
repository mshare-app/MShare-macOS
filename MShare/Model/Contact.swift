//
//  Contact.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import Foundation
import SwiftData

@Model
class Contact: Identifiable {
    let id = UUID()
    var name: String
    var pubkey: String
    
    var isPubkeyValid: Bool {
        get {
            for ch in pubkey {
                if !ch.isHexDigit {
                    return false
                }
            }

            return pubkey.count == 91 * 2
        }
    }

    init(name: String, pubkey: String) {
        self.name = name
        self.pubkey = pubkey
    }

    static func example() -> Contact {
        return Contact(name: "John Ritchie", pubkey: "DEADBEEF")
    }
    
    static func examples() -> [Contact] {
        return [
            Contact(name: "John Ritchie", pubkey: "DEADBEEF"),
            Contact(name: "Alice Briggs", pubkey: "DEDEBEBE")
        ]
    }
}

//
//  Contact.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import Foundation

struct Contact: Identifiable, Hashable {
    var id: UUID
    var name: String
    var pubkey: String
    
    init(name: String, pubkey: String) {
        self.id = UUID()
        self.name = name
        self.pubkey = pubkey
    }
    
    func hash(into hasher: inout Hasher) {
        hasher.combine(id)
    }
    
    static func == (lhs: Contact, rhs: Contact) -> Bool {
        return lhs.id == rhs.id
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

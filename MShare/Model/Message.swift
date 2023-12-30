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
    
    static func example() -> Message {
        return Message(from: .user, message: "Hello, world!")
    }
    
    static func examples() -> [Message] {
        return [
            Message(from: .user, message: "Hello!"),
            Message(from: .notUser, message: "Hey!"),
            Message(from: .user, message: "I'm good!"),
            Message(from: .user, message: "Hbu?"),
            Message(from: .notUser, message: "I'm good too!")
        ]
    }
}

//
//  MessageView.swift
//  MShare
//
//  Created by Jithin Renji on 12/29/23.
//

import SwiftUI

struct MessageView: View {
    @Binding var message: Message
    @State private var shouldChangeCursor = false
    @Environment(\.colorScheme) var colorScheme

    var body: some View {
        // Quick and dirty. TODO: Cleanup
        Text(message.message)
            .textSelection(.enabled)
            .padding(8)
            .foregroundStyle(message.from == .user ? .white : (colorScheme == .dark ? .white : .black))
            .background(message.from == .user ? .accent : (colorScheme == .dark ? .black : .gray.opacity(0.25)))
            .cornerRadius(10)
    }
}

#Preview {
    MessageView(message: .constant(Message.example()))
        .padding()
}

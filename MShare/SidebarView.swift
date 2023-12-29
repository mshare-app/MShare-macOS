//
//  SidebarView.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI

struct SidebarView: View {
    
    var body: some View {
        List {
            Section("Pinned") {
                Label("Pin a chat to see it here.", systemImage: "info.circle.fill")
            }
            
            Section("Chats") {
                
            }
        }
        .toolbar {
            Button {
                let _ = print("Add new contact.")
            } label: {
                Label("New Contact", systemImage: "person.crop.circle.badge.plus")
            }
            .help("New Contact")
        }
    }
}

#Preview {
    SidebarView()
        .listStyle(.sidebar)
}

//
//  ContentView.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI

struct ContentView: View {
    var hasContacts = false;
    var body: some View {
        NavigationSplitView {
            SidebarView()
                .frame(minWidth: 250)
        } detail: {
            if hasContacts {
                ChatView()
                    .frame(minWidth: 500, idealWidth: 800, minHeight: 500, idealHeight: 700)
            } else {
                WelcomeView()
            }
        }
    }
}

#Preview {
    ContentView()
}

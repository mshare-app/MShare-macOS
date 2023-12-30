//
//  WelcomeView.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI

struct WelcomeView: View {
    var body: some View {
        Image("MShare")
            .resizable()
            .frame(width: 300, height: 300)
        
        Label("Create a new contact to get started.", systemImage: "arrow.up.left.circle")
    }
}

#Preview {
    WelcomeView()
}

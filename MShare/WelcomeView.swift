//
//  WelcomeView.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI
import SwiftData

struct WelcomeView: View {
  var labelString = "Create a new contact to get started."
  var systemImage = "arrow.up.left.circle"

  var body: some View {
    Image("MShare")
      .resizable()
      .frame(width: 300, height: 300)
    
    Label(labelString, systemImage: systemImage)
  }
}

#Preview {
  WelcomeView()
}

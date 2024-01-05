//
//  MShareApp.swift
//  MShare
//
//  Created by Jithin Renji on 12/28/23.
//

import SwiftUI

@main
struct MShareApp: App {
  var body: some Scene {
    Window("MShare", id: "main") {
      ContentView()
        .onAppear {
          NSWindow.allowsAutomaticWindowTabbing = false
        }
    }
    .commands {
      CommandGroup(replacing: .appInfo) {
        Button("About MShare") {
          NSApplication.shared.orderFrontStandardAboutPanel(
            options: [
              NSApplication.AboutPanelOptionKey.applicationVersion: NSString("v0.1 [alpha]"),
              NSApplication.AboutPanelOptionKey.credits: NSAttributedString(
                string: "MShare is free software licensed under the GNU GPLv3.",
                attributes: [
                  NSAttributedString.Key.font: NSFont.systemFont(ofSize: NSFont.smallSystemFontSize)
                ]
              ),
              NSApplication.AboutPanelOptionKey(rawValue: "Copyright"): "Copyright © 2023-2024 Jithin Renji.",
              NSApplication.AboutPanelOptionKey.version: NSString("")
            ]
          )
        }
      }
    }
    .modelContainer(for: Contact.self)
  }
}

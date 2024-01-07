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

          let msharedProcess = Process()
          let helper = Bundle.main.path(forAuxiliaryExecutable: "mshared")
          msharedProcess.executableURL = URL(fileURLWithPath: helper!)

          do {
            try msharedProcess.run()
          } catch {
            print("Failed to run mshared: \(error.localizedDescription)")
            fatalError()
          }

          for application in NSWorkspace.shared.runningApplications {
            if application.executableURL?.absoluteString == "mshared" {
              print("Yes!: \(String(describing: application.executableURL))")
            }
          }
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

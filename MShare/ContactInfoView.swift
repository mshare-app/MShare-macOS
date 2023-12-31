//
//  ContactInfoView.swift
//  MShare
//
//  Created by Jithin Renji on 12/29/23.
//

import SwiftUI
import SwiftData

struct ContactInfoView: View {
//    @Environment(\.modelContext) var modelContext
    @State var contact: Contact

    var body: some View {
        Form {
            TextField("Name", text: $contact.name)
            TextField("Public Key", text: $contact.pubkey)
        }
        .textFieldStyle(.roundedBorder)
        .formStyle(.columns)
    }
}

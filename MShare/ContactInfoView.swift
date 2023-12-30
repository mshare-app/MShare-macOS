//
//  ContactInfoView.swift
//  MShare
//
//  Created by Jithin Renji on 12/29/23.
//

import SwiftUI

struct ContactInfoView: View {
    @Binding var contact: Contact

    var body: some View {
        Form {
            TextField("Name", text: $contact.name)
            TextField("Public Key", text: $contact.pubkey)
        }
        .textFieldStyle(.roundedBorder)
        .formStyle(.columns)
    }
}

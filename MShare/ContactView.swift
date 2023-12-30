//
//  SideChatView.swift
//  MShare
//
//  Created by Jithin Renji on 12/29/23.
//

import SwiftUI

struct ContactView: View {
    @Binding var contact: Contact

    var body: some View {
        VStack {
            HStack {
                VStack{
                    Image(systemName: "person.crop.circle.fill")
                        .resizable()
                        .frame(width: 32, height: 32)
                }
                .padding([.leading, .trailing], 10)
                
                VStack(alignment: .leading) {
                    Text(contact.name)
                    Text(contact.pubkey)
                        .foregroundStyle(.gray)
                }
            }
        }
        .padding(.vertical, 8)
    }
}

#Preview {
    ContactView(contact: .constant(Contact.example())/*, selectedContactName: .constant("")*/)
        .padding()
}

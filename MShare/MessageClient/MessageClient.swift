//
//  MessageClient.swift
//  MShare
//
//  Created by Jithin Renji on 1/1/24.
//

import Foundation
import Network
import Combine

// https://gist.github.com/michael94ellis/92828bba252ccabd071279be098e26e6
class UDPListener: ObservableObject {
  var listener: NWListener?
  var connection: NWConnection?
  var queue = DispatchQueue.global(qos: .userInitiated)

  @Published private(set) public var messageReceived: Data?
  @Published private(set) public var isReady: Bool = false
  @Published public var listening: Bool = true

  convenience init(on port: Int) {
    self.init(on: NWEndpoint.Port(integerLiteral: NWEndpoint.Port.IntegerLiteralType(port)))
  }

  init(on port: NWEndpoint.Port) {
    let params = NWParameters.udp
    params.allowFastOpen = true

    self.listener = try? NWListener(using: params, on: port)
    self.listener?.stateUpdateHandler = { update in
      switch update {
      case .ready:
        self.isReady = true
        print("Listener connected to port \(port)")

      case .failed, .cancelled:
        self.listening = false
        self.isReady = false
        print("Listener disconnected from port \(port)")

      default:
        print("Listener connecting to port \(port)...")
      }
    }

    self.listener?.newConnectionHandler = { connection in
      print("Listener receiving new message")
      self.createConnection(connection: connection)
    }

    self.listener?.start(queue: self.queue)
  }

  func createConnection(connection: NWConnection) {
    self.connection = connection
    self.connection?.stateUpdateHandler = { (newState) in
      switch (newState) {
      case .ready:
        print("Listener ready to receive message - \(connection)")
        self.receive()

      case .cancelled, .failed:
        print("Listener failed to receive message - \(connection)")
        self.listener?.cancel()
        self.listening = false

      default:
        print("Listener waiting to receive message - \(connection)")
      }
    }

    self.connection?.start(queue: .global())
  }

  func receive() {
    self.connection?.receiveMessage { data, context, isComplete, error in
      if let unwrappedError = error {
        print("Error: NWError received in \(#function) - \(unwrappedError)")
        return
      }

      guard isComplete, let data = data else {
        print("Error: Received nil Data with context - \(String(describing: context))")
        return
      }

      DispatchQueue.main.async {
        self.messageReceived = data
      }

      if self.listening {
        self.receive()
      }
    }
  }

  func cancel() {
    self.listening = false
    self.connection?.cancel()
  }
}

class MessageClient: ObservableObject {
  private var sfd: Int32
  private(set) public var listener: UDPListener

  public enum MessageClientError: Error {
    case startupError(what: String = "Client startup error.")
    case sendError(what: String = "Client send error.")
  }

  init() {
    sfd = socket(AF_INET, SOCK_DGRAM, 0)
    if sfd == -1 {
      print("Will not send messages.")
    }

    listener = UDPListener(on: 3001)
  }

  func sendPacket(packet: Packet) throws {
    guard let serialized = packet.serialize() else {
      throw MessageClientError.sendError(what: "Invalid packet (check pubkeys).")
    }

    guard serialized.count < 4096 else {
      throw MessageClientError.sendError(what: "Message too long.")
    }

    let errcode = msclient_send_packet(sfd, serialized.cString(using: .utf8))
    if errcode != 0 {
      throw MessageClientError.sendError(what: "Send failed. Error code: \(errcode)")
    }
  }
}

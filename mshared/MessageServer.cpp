#include "MessageServer.hpp"
#include "Packet.hpp"
#include "Logger.hpp"

#include "cryptopp/hex.h"

#include <fstream>
#include <thread>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

namespace MShare {

ServerStartupError::ServerStartupError(std::string what_str): what_str_(what_str) { }

const char* ServerStartupError::what() const noexcept {
  return what_str_.c_str();
}

MessageServer::MessageServer(CryptoContext& cctx): cctx_{cctx}, port_{3000} {
  /* TODO: Switch to TCP. */
  if (fs::exists(cctx_.get_msdir())) {
    std::ifstream known_peers_file(cctx_.get_msdir() / "known_peers.txt");
    std::string line;
    while (std::getline(known_peers_file, line)) {
      known_peers_.push_back(line);
    }
  } else {
    warn() << "No known peers.\n";
  }

  status() << "Starting message server...\n";
  sfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd_ == -1) {
    throw ServerStartupError("Server socket creation failed.");
  }

  int opt = 1;
  if (setsockopt(sfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    throw ServerStartupError("Server setsockopt() failed.");
  }

  sockaddr_in saddr = {
    .sin_family = AF_INET,
    .sin_port = htons(port_),
    .sin_addr = INADDR_ANY
  };

  if (bind(sfd_, (sockaddr *) &saddr, sizeof(saddr)) < 0) {
    throw ServerStartupError("Server bind() failed.");
  }

  // We're alive.
  status() << "Done.\n";
  status() << "MShare server running on 0.0.0.0:" << port_ << '\n';
  status() << "Begin server main loop:\n";
  main_loop();
}

MessageServer::~MessageServer() {
  close(sfd_);
  status() << "Server bye.\n";
}

void MessageServer::main_loop() {
  if (listen(sfd_, 3) == -1) {
    error() << "Listen failed.\n";
    return;
  }

  while (true) {
    sockaddr_in csaddr;
    socklen_t caddr_sz = sizeof(csaddr);
    int csfd = accept(sfd_, (sockaddr *) &csaddr, &caddr_sz);
    if (csfd == -1) {
      error() << "Accepting connection from " << inet_ntoa(csaddr.sin_addr) << " failed.\n";
      continue;
    }

    std::thread client_thread([csfd, this] {
      const int BUFLEN = 4096;
      char buf[BUFLEN];
      int buf_start_idx = 0;
      std::memset(buf, 0, BUFLEN);

      ssize_t total_nrecv = 0;
      while (total_nrecv != BUFLEN) {
        ssize_t nrecv = recv(csfd, &buf[buf_start_idx], BUFLEN, 0);
        if (nrecv == -1) {
          error() << "recv failed.\n";
          close(csfd);
          return;
        } else if (nrecv == 0) {
          status() << "Connection closed by peer.\n";
          close(csfd);
        }

        total_nrecv += nrecv;
        buf_start_idx += nrecv;
      }

      Packet packet(buf);
      if (packet.from_pubkey == cctx_.get_hex_pubkey()) {
        encrypt_packet(packet);
      } else if (packet.to_pubkey == cctx_.get_hex_pubkey()) {
        std::string decoded_ciphertext;
        CryptoPP::StringSource ss(
          packet.msg,
          packet.msg.length(),
          new CryptoPP::HexDecoder(
            new CryptoPP::StringSink(decoded_ciphertext)
          )
        );

        // TODO: Catch invalid ciphertext exception.
        packet.msg = cctx_.decrypt(decoded_ciphertext);
        status() << "Message: " << packet.msg << '\n';
      }

      forward(packet);

      // TODO: Send to GUI.

      close(csfd);
    });

    client_thread.detach();
  }
}

void MessageServer::encrypt_packet(Packet &packet) {
  std::string decoded_key;
  CryptoPP::StringSource ss(
    packet.to_pubkey,
    packet.to_pubkey.size(),
    new CryptoPP::HexDecoder(
      new CryptoPP::StringSink(decoded_key)
    )
  );

  CryptoPP::AutoSeededRandomPool prng;

  Encryptor enc;
  CryptoPP::StringSource pubkey_ss(decoded_key, decoded_key.size());
  enc.AccessPublicKey().Load(pubkey_ss);
  enc.GetPublicKey().ThrowIfInvalid(prng, 3);

  std::string ciphertext;
  CryptoPP::StringSource enc_ss(
    packet.msg,
    true,
    new CryptoPP::PK_EncryptorFilter(
      prng,
      enc,
      new CryptoPP::StringSink(ciphertext)
    )
  );

  packet.msg = ciphertext;
}

void MessageServer::forward(Packet &packet) {
  bool sent_to_at_least_one = false;

  int csfd = socket(AF_INET, SOCK_STREAM, 0);
  if (csfd == -1) {
    error() << "forward() socket creation failed.";
    return;
  }

  sockaddr_in saddr = {
    .sin_family = AF_INET,
    .sin_port = htons(port_)
  };

  std::string sbuf = packet.serialize();
  const int BUFSIZE = 4096;
  char buf[BUFSIZE];
  std::memset(buf, 0, BUFSIZE);
  std::strcpy(buf, sbuf.c_str());
  for (std::string &peer : known_peers_) {
    if (inet_pton(AF_INET, peer.c_str(), &saddr.sin_addr) != 1) {
      error() << '"' << peer << '"' << " could not be parsed.\n";
      continue;
    }

    if (connect(csfd, (sockaddr *) &saddr, sizeof(saddr)) < 0) {
      error() << peer << ": Connection failed.\n";
      continue;
    }

    // TODO: Do not forward to localhost if it is somehow a known peer.
    ssize_t ret = send(csfd, buf, BUFSIZE, 0);
    if (ret == -1 || ret != BUFSIZE) {
      perror("");
      error() << peer << ": Full send failed (" << ret << ").\n";
      continue;
    }

    sent_to_at_least_one = true;
  }

  if (sent_to_at_least_one) {
    status() << "Message forwarded.\n";
  } else {
    warn() << "Please check " << cctx_.get_msdir() / "known_peers.txt"
           << " (invalid peers/all peers offline)" << '\n';
  }
}

} // namespace MShare

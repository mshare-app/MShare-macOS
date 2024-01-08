#include "MessageServer.hpp"
#include "Packet.hpp"
#include "Logger.hpp"

#include "cryptopp/hex.h"

#include <fstream>

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
  sfd_ = socket(AF_INET, SOCK_DGRAM, 0);
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

  if (bind(sfd_, (sockaddr*) &saddr, sizeof(saddr)) < 0) {
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
  struct sockaddr_in client;
  socklen_t caddr_sz = 0;

  const int BUFLEN = 4096;
  std::vector<char> buf(BUFLEN);
  std::string sbuf;
  size_t nrecv = 0;
  while ((nrecv = recvfrom(sfd_, &buf[0], BUFLEN - 1, 0, (struct sockaddr*) &client, &caddr_sz)) > 0) {
    sbuf.append(buf.cbegin(), buf.cend());
    sbuf.resize(std::strlen(buf.data()));

    MShare::Packet packet(sbuf);

    if (packet.from_pubkey == cctx_.get_hex_pubkey()) {
      try {
        encrypt_packet(packet);
      } catch (CryptoPP::InvalidCiphertext &e) {
        warn() << "Skipping invalid packet.\n";
        continue;
      }
    }

    forward(packet);
    status() << "Packet ciphertext hash: " << to_hex(sha3_256(packet.msg)) << " " << packet.serialize().size() << '\n';

//    status() << "New message from " << inet_ntoa(client.sin_addr) << ": " << packet.msg << '\n';
    std::fill(buf.begin(), buf.end(), 0);
    sbuf.clear();
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
  sockaddr_in saddr = {
    .sin_family = AF_INET,
    .sin_port = htons(port_)
  };

  for (std::string &peer : known_peers_) {
    if (inet_pton(AF_INET, peer.c_str(), &saddr.sin_addr) != 1) {
      error() << '"' << peer << '"' << " could not be parsed.\n";
      continue;
    }

    // TODO: Do not forward to localhost if it is somehow a known peer.
    std::string sbuf = packet.serialize();
    ssize_t ret = sendto(sfd_, sbuf.c_str(), sbuf.size(), 0, (struct sockaddr *) &saddr, sizeof(saddr));
    if (ret == -1 || ret != sbuf.size()) {
      error() << peer << ": Full send failed (" << ret << ").\n";
      continue;
    }

    status() << "Send succeeded! " << ret << '\n';

    sent_to_at_least_one = true;
  }

  if (sent_to_at_least_one) {
    status() << "Message forwarded.\n";
  } else {
    warn() << "Please check " << cctx_.get_msdir() / "known_peers.txt" << '\n';
  }
}

} // namespace MShare

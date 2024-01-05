#include "MessageServer.hpp"
#include "Packet.hpp"
#include "Logger.hpp"

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
    // if (packet.pubkey_hash != cctx_.get_pubkey_hash()) {
    //   // TODO: Forward.
    //   continue;
    // }

    status() << "New message from " << inet_ntoa(client.sin_addr) << ": " << packet.msg << '\n';
    std::fill(buf.begin(), buf.end(), 0);
    sbuf.clear();
  }
}

} // namespace MShare

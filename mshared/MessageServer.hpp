#ifndef MSMESSAGE_SERVER_HPP
#define MSMESSAGE_SERVER_HPP

#include "Crypto.hpp"
#include "Packet.hpp"

#include <string>
#include <stdexcept>

#include <stdint.h>

namespace MShare {

class ServerStartupError : public std::exception {
public:
  ServerStartupError(std::string what_str);
  const char* what() const noexcept override;

private:
  std::string what_str_;
};

class MessageServer {
public:
  MessageServer(CryptoContext& cctx);
  ~MessageServer();

private:
  uint16_t port_;
  int sfd_;
  CryptoContext& cctx_;
  std::vector<std::string> known_peers_;

  void main_loop();
  void encrypt_packet(Packet &packet);
  void forward(Packet &packet);
  void forward_to_client(Packet &packet);
};

} // namespace MShare

#endif  // MSMESSAGE_SERVER_HPP

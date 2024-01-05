#ifndef MSPACKET_HPP
#define MSPACKET_HPP

#include <stdexcept>

#include <stddef.h>
#include <string>

namespace MShare {

class PacketizationError : public std::exception {
public:
  PacketizationError(std::string what_str);

  const char* what() const noexcept override;

private:
  std::string what_str_;
};

struct Packet {
  // Attempt to deserialize MShare packet received from the network.
  Packet(std::string serialized);
  Packet();

  std::string serialize();

  std::string version;
  std::string from_pubkey;
  std::string to_pubkey;
  std::string msg;
};

} // namespace MShare

#endif  // MSPACKET_HPP

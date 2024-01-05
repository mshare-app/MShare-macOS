#include "Logger.hpp"
#include "Packet.hpp"

namespace MShare {

PacketizationError::PacketizationError(std::string what_str): what_str_(what_str) { }

const char* PacketizationError::what() const noexcept {
  return what_str_.c_str();
}

Packet::Packet(std::string serialized) {
  using MShare::status;

  std::vector<std::string> fields;

  for (int i = 0; i < 3; i++) {
    size_t delim_idx = serialized.find('_');
    if (delim_idx == std::string::npos) {
      throw PacketizationError("No delimiter in serialized packet.");
    }

    fields.push_back(serialized.substr(0, delim_idx));
    serialized.erase(0, delim_idx + 1);
  }

  version = fields[0];
  from_pubkey = fields[1];
  to_pubkey = fields[2];
  msg = serialized;
}

Packet::Packet(): version("0.1") { }

std::string Packet::serialize() {
  return version + "_" + from_pubkey + "_" + to_pubkey + "_" + msg;
}

} // namespace MShare

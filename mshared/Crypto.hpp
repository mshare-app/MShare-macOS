#ifndef MSCRYPTO_CONTEXT_HPP
#define MSCRYPTO_CONTEXT_HPP

#include "cryptopp/osrng.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/sha3.h"

#include <iostream>
#include <streambuf>
#include <string_view>
#include <filesystem>

namespace MShare
{

namespace fs = std::filesystem;

class CryptoContext {
public:
  CryptoContext(std::string &msdir);

  std::string encrypt(std::string input);
  std::string decrypt(std::string input);
//  std::string get_pubkey_hash();
  std::string get_hex_pubkey() const;
  fs::path get_msdir() const;

private:
  using Decyptor = CryptoPP::ECIES<CryptoPP::ECP>::Decryptor;
  using Encryptor = CryptoPP::ECIES<CryptoPP::ECP>::Encryptor;

  fs::path msdir_;

  CryptoPP::AutoSeededRandomPool prng_;
  Decyptor decryptor_;
  Encryptor encryptor_;

  void save_keys();
  void load_keys();

  friend std::ostream& operator<<(std::ostream& os, CryptoContext& cc);
};

std::string to_hex(const std::string& s);
std::string sha3_256(const std::string& input);

} // namespace MShare

#endif  // MSCRYPTO_CONTEXT_HPP

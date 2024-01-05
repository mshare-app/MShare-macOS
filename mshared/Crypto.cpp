#include "Crypto.hpp"
#include "Logger.hpp"

#include "cryptopp/files.h"
#include "cryptopp/hex.h"
#include "cryptopp/filters.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/asn.h"
#include "cryptopp/oids.h"

#include <iostream>
#include <filesystem>

#ifdef __APPLE__
  #include <stdexcept>
  #include <sysdir.h>
  #include <glob.h>
#endif

namespace MShare {

CryptoContext::CryptoContext(std::string &msdir): msdir_(msdir) {
  namespace fs = std::filesystem;

  if (!fs::exists(msdir_)) {
    fs::create_directory(msdir_);
    warn() << "Creating directory " << msdir_ << '\n';
  }

  bool generate_new_keys = false;
  if (!fs::exists(msdir_ / "pubkey") || !fs::exists(msdir_ / "privkey")) {
    warn() << "No keys in " << msdir_ << ". Generating...\n";
    generate_new_keys = true;
  } else {
    status() << "Loading keys...\n";
  }

//  CryptoPP::ECIES<CryptoPP::ECP> d0;

  if (generate_new_keys) {
    decryptor_ = Decyptor(prng_, CryptoPP::ASN1::secp256r1());
    encryptor_ = Encryptor(decryptor_);
    save_keys();
  } else {
    load_keys();
  }

  status() << "Done.\n";
}

std::string CryptoContext::encrypt(std::string input) {
  std::string ciphertext;
  CryptoPP::StringSource ss(
    input,
    true,
    new CryptoPP::PK_EncryptorFilter(
      prng_,
      encryptor_,
      new CryptoPP::StringSink(ciphertext)
    )
  );

  return ciphertext;
}

std::string CryptoContext::decrypt(std::string input) {
  std::string plaintext;
  CryptoPP::StringSource ss(
    input,
    true,
    new CryptoPP::PK_DecryptorFilter(
      prng_,
      decryptor_,
      new CryptoPP::StringSink(plaintext)
    )
  );

  return plaintext;
}

std::string CryptoContext::get_pubkey_hash() {
  const CryptoPP::ECPPoint& point = encryptor_.GetKey().GetPublicElement();

  std::string hash;
  CryptoPP::SHA3_256 hasher;

  std::vector<CryptoPP::byte> x_bytes(point.x.ByteCount());
  std::vector<CryptoPP::byte> y_bytes(point.y.ByteCount());
  hasher.Update(x_bytes.data() , x_bytes.size());
  hasher.Update(y_bytes.data(), y_bytes.size());
  hash.resize(hasher.DigestSize());
  hasher.Final((CryptoPP::byte*) &hash[0]);

  return hash;
}

void CryptoContext::save_keys() {
  CryptoPP::FileSink pubkey_sink((msdir_ / "pubkey").c_str());
  encryptor_.GetPublicKey().Save(pubkey_sink);

  CryptoPP::FileSink privkey_sink((msdir_ / "privkey").c_str());
  decryptor_.GetPrivateKey().Save(privkey_sink);
}

void CryptoContext::load_keys() {
  CryptoPP::FileSource pubkey_source((msdir_ / "pubkey").c_str(), true);
  encryptor_.AccessPublicKey().Load(pubkey_source);
  encryptor_.GetPublicKey().ThrowIfInvalid(prng_, 3);

  CryptoPP::FileSource privkey_source((msdir_ / "privkey").c_str(), true);
  decryptor_.AccessPrivateKey().Load(privkey_source);
  decryptor_.GetPrivateKey().ThrowIfInvalid(prng_, 3);
}

// Taken from https://www.cryptopp.com/wiki/Elliptic_Curve_Integrated_Encryption_Scheme
namespace {

void print_privkey(const CryptoPP::DL_PrivateKey_EC<CryptoPP::ECP>& key, std::ostream& out = std::cout) {
  // Group parameters
  const CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP>& params = key.GetGroupParameters();
  // Base precomputation (for public key calculation from private key)
  const CryptoPP::DL_FixedBasePrecomputation<CryptoPP::ECPPoint>& bpc = params.GetBasePrecomputation();
  // Public Key (just do the exponentiation)
  const CryptoPP::ECPPoint point = bpc.Exponentiate(params.GetGroupPrecomputation(), key.GetPrivateExponent());

  out << "Modulus: " << std::hex << params.GetCurve().GetField().GetModulus() << '\n';
  out << "Cofactor: " << std::hex << params.GetCofactor() << '\n';

  out << "Coefficients" << '\n';
  out << "  A: " << std::hex << params.GetCurve().GetA() << '\n';
  out << "  B: " << std::hex << params.GetCurve().GetB() << '\n';

  out << "Base Point" << '\n';
  out << "  x: " << std::hex << params.GetSubgroupGenerator().x << '\n';
  out << "  y: " << std::hex << params.GetSubgroupGenerator().y << '\n';

  out << "Public Point" << '\n';
  out << "  x: " << std::hex << point.x << '\n';
  out << "  y: " << std::hex << point.y << '\n';

  out << "Private Exponent (multiplicand): " << '\n';
  out << "  " << std::hex << key.GetPrivateExponent() << '\n';
}

void print_pubkey(const CryptoPP::DL_PublicKey_EC<CryptoPP::ECP>& key, std::ostream& out) {
  // Group parameters
  const CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP>& params = key.GetGroupParameters();
  // Public key
  const CryptoPP::ECPPoint& point = key.GetPublicElement();

  out << "Modulus: " << std::hex << params.GetCurve().GetField().GetModulus() << '\n';
  out << "Cofactor: " << std::hex << params.GetCofactor() << '\n';

  out << "Coefficients" << '\n';
  out << "  A: " << std::hex << params.GetCurve().GetA() << '\n';
  out << "  B: " << std::hex << params.GetCurve().GetB() << '\n';

  out << "Base Point" << '\n';
  out << "  x: " << std::hex << params.GetSubgroupGenerator().x << '\n';
  out << "  y: " << std::hex << params.GetSubgroupGenerator().y << '\n';

  out << "Public Point" << '\n';
  out << "  x: " << std::hex << point.x << '\n';
  out << "  y: " << std::hex << point.y << '\n';
}

} // namespace

std::ostream& operator<<(std::ostream& os, CryptoContext& cc) {
  os << "-----PRIVATE KEY-----\n";
  print_privkey(cc.decryptor_.GetKey(), os);
  os << "\n-----PUBLIC KEY-----\n";
  print_pubkey(cc.encryptor_.GetKey(), os);

  return os;
}

std::string to_hex(const std::string& s) {
  std::string hex_encoded;
  CryptoPP::StringSource ss(
    s,
    s.size(),
    new CryptoPP::HexEncoder(
      new CryptoPP::StringSink(hex_encoded),
      false
    )
  );

  return hex_encoded;
}

std::string sha3_256(const std::string& input) {
  std::string hash;
  CryptoPP::SHA3_256 hasher;
  hasher.Update((const unsigned char*) input.data(), input.length());
  hash.resize(hasher.DigestSize());
  hasher.Final((unsigned char*) &hash[0]);

  return hash;
}


} // namespace MShare

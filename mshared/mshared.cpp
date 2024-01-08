//
//  mshared.cpp
//  mshared
//
//  Created by Jithin Renji on 1/5/24.
//

#include "mshared.hpp"
#include "Logger.hpp"
#include "Crypto.hpp"
#include "MessageServer.hpp"

#include <iostream>

using MShare::status;
using MShare::warn;
using MShare::error;

int mshare_start(std::string app_dir) {
  status() << "Got app dir: " << app_dir << '\n';

  MShare::CryptoContext cc(app_dir);
  status() << "Public key: " << cc.get_hex_pubkey() << '\n';

  try {
    MShare::MessageServer server(cc);
  } catch(MShare::ServerStartupError &e) {
    error() << e.what() << '\n';
  }

  return 0;
}

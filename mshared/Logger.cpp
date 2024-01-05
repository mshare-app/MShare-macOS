#include "Logger.hpp"
#include <ctime>

namespace MShare {

namespace {

enum LogLevel {
  STATUS,
  WARN,
  ERROR
};

std::ostream& operator<<(std::ostream& os, LogLevel& level) {
  switch (level) {
  case STATUS:
    os << "STATUS";
    break;

  case WARN:
    os << "WARN";
    break;

  default:
    os << "ERROR";
    break;
  }

  return os;
}

} // namespace

std::ostream& log(std::ostream& os, LogLevel level) {
  std::time_t now = std::time(0);
  std::tm* local = std::localtime(&now);
  std::string localtime_str = std::asctime(local);

  std::string level_color;
  switch (level) {
  case STATUS:
    level_color = "\e[0;32m";
    break;

  case WARN:
    level_color = "\e[0;33m";
    break;

  default:
    level_color = "\e[0;31m";
  }

  os << "[" << localtime_str.substr(0, localtime_str.length() - 1) << " " << level_color << level << "\e[0m" << "] ";

  return os;
}

std::ostream& status(std::ostream& os) {
  return log(os, STATUS);
}

std::ostream& warn(std::ostream& os) {
  return log(os, WARN);
}

std::ostream& error(std::ostream& os) {
  return log(os, ERROR);
}


} // namespace MShare

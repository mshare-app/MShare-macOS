#include "ArgParser.hpp"

#include <iomanip>
#include <algorithm>
#include <string>

namespace MShare {

UnknownOptionError::UnknownOptionError(std::string option): option_(option) {
  what_str_ = "Unknown option encountered during command-line parsing: ";
  what_str_ += option_;
}

const char* UnknownOptionError::what() const noexcept {
  return what_str_.c_str();
}

NoValueForOptionError::NoValueForOptionError(std::string option): option_(option) {
  what_str_ = "No value for option: ";
  what_str_ += option_;
}

const char* NoValueForOptionError::what() const noexcept {
  return what_str_.c_str();
}

UnexpectedArgumentError::UnexpectedArgumentError(std::string arg): arg_(arg) {
  what_str_ = "Unexpected argument encountered during command-line parsing: ";
  what_str_ += arg_;
}

const char* UnexpectedArgumentError::what() const noexcept {
  return what_str_.c_str();
}

ArgParser::ArgParser(int argc, char *argv[], const OptionList& options): progname_(argv[0]), options_(options) {
  // For convenience. Ignoring argv[0] because we already have it in progname_.
  for (int i = 1; i < argc; i++) {
    original_args_.push_back(argv[i]);
  }

  parse();
}

ParsedOptions ArgParser::get() {
  return parsed_options_;
}

void ArgParser::print_help(std::ostream& os) {
  auto longest_option = std::max_element(options_.begin(), options_.end(), [](const Option& o1, const Option& o2) {
    return o1.name.length() < o2.name.length();
  });

  size_t col_width = longest_option->name.length() + (longest_option->expected_val_type != Option::NONE ? 7 : 0) + 4;

  os << "Usage: " << progname_ << " [options]\n";
  os  << "Options:\n";
  for (Option& option : options_) {
    std::string exp_val_str;
    switch (option.expected_val_type) {
    case Option::STRING:
      exp_val_str = " STRING";
      break;

    case Option::INT:
      exp_val_str = " INTEGER";
      break;

    case Option::FLOAT:
      exp_val_str = " FLOAT";
      break;

    default:  // Option::NONE
      break;
    }

    os << "  --" << std::setw(static_cast<int>(col_width)) << std::left << (option.name + exp_val_str) << option.help << '\n';
  }
}

void ArgParser::parse() {
  bool expecting_arg = false;
  std::string cur_opt_name;
  for (std::string& arg : original_args_) {

    if (arg.length() > 2 && arg.substr(0, 2) == "--" && expecting_arg) {
      throw NoValueForOptionError(arg);
    } else if (arg.length() > 2 && arg.substr(0, 2) == "--" && !expecting_arg) {
      std::string option_name = arg.substr(2);
      auto found_option = std::find_if(options_.begin(), options_.end(), [&option_name](const Option& option) {
        return option.name == option_name;
      });
      if (found_option == options_.end()) {
        throw UnknownOptionError(arg);
      }

      if (found_option->type == Option::FLAG) {
        parsed_options_[option_name] = "true";
      } else if (found_option->type == Option::KWARG) {
        expecting_arg = true;
        cur_opt_name = found_option->name;
      }
    } else if (expecting_arg) {
      // Only handles strings for now.
      // TODO: Handle non-string types.
      parsed_options_[cur_opt_name] = arg;
      cur_opt_name.clear();
      expecting_arg = false;
    } else {
      throw UnexpectedArgumentError(arg);
    }
  }

  if (expecting_arg) {
    throw NoValueForOptionError(original_args_.at(original_args_.size() - 1));
  }
}

} // namespace MShare

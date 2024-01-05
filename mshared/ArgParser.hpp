#ifndef MSARG_PARSER_HPP
#define MSARG_PARSER_HPP

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <any>
#include <unordered_map>
#include <stdexcept>

namespace MShare {

class UnknownOptionError : public std::exception {
public:
  UnknownOptionError(std::string option);
  const char* what() const noexcept override;

private:
  std::string option_;
  std::string what_str_;
};

class NoValueForOptionError : public std::exception {
public:
  NoValueForOptionError(std::string option);
  const char* what() const noexcept override;

private:
  std::string option_;
  std::string what_str_;
};

class UnexpectedArgumentError : public std::exception {
public:
  UnexpectedArgumentError(std::string arg);
  const char* what() const noexcept override;

private:
  std::string arg_;
  std::string what_str_;
};

struct Option {
  std::string name;

  enum Type {
    FLAG,   // eg. --verbose
    // ARG,    // eg. -n 10
    KWARG   // eg. --number 10
  } type;

  enum ExpectedValType {
    INT,
    FLOAT,
    STRING,
    NONE
  } expected_val_type = NONE;

  std::string help;
};

using OptionList = std::vector<Option>;
using ParsedOptions = std::unordered_map<std::string, std::any>;

class ArgParser {
public:
  ArgParser(int argc, char *argv[], const OptionList& options);

  ParsedOptions get();
  void print_help(std::ostream& os = std::cout);

private:
  std::string progname_;
  OptionList options_;
  std::vector<std::string> original_args_;
  ParsedOptions parsed_options_;

  void parse();
};

} // namespace MShare

#endif

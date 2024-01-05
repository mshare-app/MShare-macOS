#ifndef MSLOGGER_HPP
#define MSLOGGER_HPP

#include <iostream>

namespace MShare {

std::ostream &status(std::ostream &os = std::cerr);
std::ostream &warn(std::ostream &os = std::cerr);
std::ostream& error(std::ostream& os = std::cerr);

}

#endif


#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <string>
#include <cassert>
#include <limits>

void
remove_trailing_whitespaces(std::string& str);

std::size_t 
get_ranged_input_inclusive(std::size_t low, std::size_t high);

void
print_programm_name();

#endif // HELPERS_H
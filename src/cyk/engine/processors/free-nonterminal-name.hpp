#pragma once

#include <cstring>
#include "../grammar.hpp"

std::string free_nonterminal_name(const Grammar& grammar, const std::string& start_name = "tmp");
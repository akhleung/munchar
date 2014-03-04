#ifndef MUNCHAR_LEXEMES
#define MUNCHAR_LEXEMES

#include "munchar_stateful.hpp"
#include <cctype>

namespace Munchar {
  namespace Lexemes {

    constexpr auto _            = Any { };
    constexpr auto letter       = P(std::isalpha);
    constexpr auto alphanumeric = P(std::isalnum);
    constexpr auto digit        = P(std::isdigit);
    constexpr auto hex_digit    = P(std::isxdigit);
    constexpr auto id_start     = letter | '_'_lit;
    constexpr auto id_body      = alphanumeric | '_'_lit;
    constexpr auto identifier   = id_start & *id_body;
    constexpr auto hex_color    = '#'_lit & between<3,6>(hex_digit);
    constexpr auto sign         = '+'_lit | '-'_lit;
    constexpr auto number       = ~sign & ((*digit & '.'_lit & +digit)|+digit);

  }
}

#endif
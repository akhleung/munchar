#ifndef MUNCHAR_LEXEMES
#define MUNCHAR_LEXEMES

#include "munchar.hpp"
#include <cctype>

namespace Munchar {
  namespace Lexemes {

    constexpr auto null          = '\0'_lit;
    constexpr auto tab           = '\t'_lit;
    constexpr auto newline       = '\n'_lit;
    constexpr auto linefeed      = newline;
    constexpr auto cr            = '\r'_lit;
    constexpr auto crlf          = "\r\n"_lit;

    constexpr auto space         = ' '_lit;
    constexpr auto exclamation   = '!'_lit;
    constexpr auto double_quote  = '"'_lit;
    constexpr auto quote         = double_quote;
    constexpr auto pound         = '#'_lit;
    constexpr auto hash          = pound;
    constexpr auto octothorpe    = pound;
    constexpr auto dollar        = '$'_lit;
    constexpr auto percent       = '%'_lit;
    constexpr auto ampersand     = '&'_lit;
    constexpr auto apostrophe    = '\''_lit;
    constexpr auto single_quote  = apostrophe;
    constexpr auto left_paren    = '('_lit;
    constexpr auto right_paren   = ')'_lit;
    constexpr auto asterisk      = '*'_lit;
    constexpr auto plus          = '+'_lit;
    constexpr auto comma         = ','_lit;
    constexpr auto hyphen        = '-'_lit;
    constexpr auto minus         = hyphen;
    constexpr auto period        = '.'_lit;
    constexpr auto dot           = period;
    constexpr auto slash         = '/'_lit;
    constexpr auto divide        = slash;
    constexpr auto colon         = ':'_lit;
    constexpr auto semicolon     = ';'_lit;
    constexpr auto less_than     = '<'_lit;
    constexpr auto equals        = '='_lit;
    constexpr auto greater_than  = '>'_lit;
    constexpr auto question      = '?'_lit;
    constexpr auto at            = '@'_lit;
    constexpr auto left_bracket  = '['_lit;
    constexpr auto backslash     = '\\';
    constexpr auto right_bracket = ']'_lit;
    constexpr auto caret         = '^'_lit;
    constexpr auto circumflex    = caret;
    constexpr auto underscore    = '_'_lit;
    constexpr auto backquote     = '`'_lit;
    constexpr auto left_brace    = '{'_lit;
    constexpr auto vertical_bar  = '|'_lit;
    constexpr auto pipe          = vertical_bar;
    constexpr auto right_brace   = '}'_lit;
    constexpr auto tilde         = '~'_lit;

    constexpr auto _             = Any_Char { };
    constexpr auto letter        = P(std::isalpha);
    constexpr auto alphanumeric  = P(std::isalnum);
    constexpr auto digit         = P(std::isdigit);
    constexpr auto hex_digit     = P(std::isxdigit);
    constexpr auto whitespace    = P(std::isspace);
    constexpr auto sign          = "+-"_cls;
    // constexpr auto id_start = letter | underscore;
    // constexpr auto id_body = alphanumeric | underscore;
    // constexpr auto identifier = id_start & *id_body;
    // constexpr auto number = ~sign & ((*digit & '.'_lit_lit & +digit) | +digit);
  }
}

#endif
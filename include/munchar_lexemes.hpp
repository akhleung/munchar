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
    constexpr auto lt            = less_than;
    constexpr auto lte           = "<="_lit;
    constexpr auto equals        = '='_lit;
    constexpr auto eq            = equals;
    constexpr auto greater_than  = '>'_lit;
    constexpr auto gt            = greater_than;
    constexpr auto gte           = ">="_lit;
    constexpr auto question      = '?'_lit;
    constexpr auto at            = '@'_lit;
    constexpr auto left_bracket  = '['_lit;
    constexpr auto backslash     = '\\'_lit;
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
    constexpr auto letter        = MUNCHAR_STATIC_PREDICATE(::isalpha);
    constexpr auto alphanumeric  = MUNCHAR_STATIC_PREDICATE(::isalnum);
    constexpr auto digit         = MUNCHAR_STATIC_PREDICATE(::isdigit);
    constexpr auto hex_digit     = MUNCHAR_STATIC_PREDICATE(::isxdigit);
    constexpr auto ws_char       = MUNCHAR_STATIC_PREDICATE(::isspace);
    constexpr auto whitespace    = *ws_char;
    constexpr auto sign          = "+-"_cls;
    constexpr auto id_start      = letter | underscore;
    constexpr auto id_body       = alphanumeric | underscore;
    constexpr auto identifier    = id_start^*id_body;
    constexpr auto integer       = ~sign ^ +digit;
    constexpr auto number_ne     = ~sign ^ ((*digit ^ dot ^ +digit) | +digit);
    constexpr auto number        = number_ne ^ ~("eE"_cls ^ ~sign ^ +digit);
    constexpr auto escape_seq    = backslash ^ _;
    constexpr auto d_string      = double_quote ^
                                   *(escape_seq | (!"\"\\"_cls ^ _)) ^
                                   double_quote;
    constexpr auto s_string      = single_quote ^
                                   *(escape_seq | (!"'\\"_cls ^ _)) ^
                                   single_quote;
    constexpr auto string        = d_string | s_string;
    constexpr auto eol           = newline | crlf;
    constexpr auto c_comment     = "//"_lit ^ *(!eol ^ _) ^ ~eol;
    constexpr auto cpp_comment   = "/*"_lit ^ *(!"*/"_lit ^ _) ^ "*/"_lit;
    constexpr auto sh_comment    = '#'_lit ^ *(!eol ^ _) ^ ~eol;

  }
}

#endif
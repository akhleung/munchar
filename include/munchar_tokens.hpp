#ifndef MUNCHAR_TOKENS
#define MUNCHAR_TOKENS

#include "munchar.hpp"
#include <cctype>

namespace Munchar {
  namespace Tokens {

    constexpr auto null          = CHR('\0');
    constexpr auto tab           = CHR('\t');
    constexpr auto newline       = CHR('\n');
    constexpr auto linefeed      = newline;
    constexpr auto cr            = CHR('\r');
    constexpr auto crlf          = STR("\r\n");

    constexpr auto space         = CHR(' ');
    constexpr auto exclamation   = CHR('!');
    constexpr auto double_quote  = CHR('"');
    constexpr auto quote         = double_quote;
    constexpr auto pound         = CHR('#');
    constexpr auto hash          = pound;
    constexpr auto octothorpe    = pound;
    constexpr auto dollar        = CHR('$');
    constexpr auto percent       = CHR('%');
    constexpr auto ampersand     = CHR('&');
    constexpr auto apostrophe    = CHR('\'');
    constexpr auto single_quote  = apostrophe;
    constexpr auto left_paren    = CHR('(');
    constexpr auto right_paren   = CHR(')');
    constexpr auto asterisk      = CHR('*');
    constexpr auto plus          = CHR('+');
    constexpr auto comma         = CHR(',');
    constexpr auto hyphen        = CHR('-');
    constexpr auto minus         = hyphen;
    constexpr auto period        = CHR('.');
    constexpr auto dot           = period;
    constexpr auto slash         = CHR('/');
    constexpr auto divide        = slash;
    constexpr auto colon         = CHR(':');
    constexpr auto semicolon     = CHR(';');
    constexpr auto less_than     = CHR('<');
    constexpr auto lt            = less_than;
    constexpr auto lte           = STR("<=");
    constexpr auto equals        = CHR('=');
    constexpr auto eq            = equals;
    constexpr auto greater_than  = CHR('>');
    constexpr auto gt            = greater_than;
    constexpr auto gte           = STR(">=");
    constexpr auto question      = CHR('?');
    constexpr auto at            = CHR('@');
    constexpr auto left_bracket  = CHR('[');
    constexpr auto backslash     = CHR('\\');
    constexpr auto right_bracket = CHR(']');
    constexpr auto caret         = CHR('^');
    constexpr auto circumflex    = caret;
    constexpr auto underscore    = CHR('_');
    constexpr auto backquote     = CHR('`');
    constexpr auto left_brace    = CHR('{');
    constexpr auto vertical_bar  = CHR('|');
    constexpr auto pipe          = vertical_bar;
    constexpr auto right_brace   = CHR('}');
    constexpr auto tilde         = CHR('~');

    constexpr auto _             = Any_Char { };
    constexpr auto letter        = MUNCHAR_STATIC_PREDICATE(::isalpha);
    constexpr auto alphanumeric  = MUNCHAR_STATIC_PREDICATE(::isalnum);
    constexpr auto digit         = MUNCHAR_STATIC_PREDICATE(::isdigit);
    constexpr auto hex_digit     = MUNCHAR_STATIC_PREDICATE(::isxdigit);
    constexpr auto ws_char       = MUNCHAR_STATIC_PREDICATE(::isspace);
    constexpr auto whitespace    = *ws_char;
    constexpr auto sign          = CLS("+-");
    constexpr auto id_start      = letter | underscore;
    constexpr auto id_body       = alphanumeric | underscore;
    constexpr auto identifier    = id_start^*id_body;
    constexpr auto integer       = ~sign ^ +digit;
    constexpr auto number_ne     = ~sign ^ ((*digit ^ dot ^ +digit) | +digit);
    constexpr auto number        = number_ne ^ ~(CLS("eE") ^ ~sign ^ +digit);
    constexpr auto escape_seq    = backslash ^ _;
    constexpr auto dq_string     = double_quote ^
                                   *(escape_seq | (!CLS("\"\\") ^ _)) ^
                                   double_quote;
    constexpr auto sq_string     = single_quote ^
                                   *(escape_seq | (!CLS("'\\") ^ _)) ^
                                   single_quote;
    constexpr auto string        = dq_string | sq_string;
    constexpr auto eol           = newline | crlf;
    constexpr auto cpp_comment   = STR("//") ^ *(!eol ^ _) ^ ~eol;
    constexpr auto c_comment     = STR("/*") ^ *(!STR("*/") ^ _) ^ STR("*/");
    constexpr auto sh_comment    = CHR('#') ^ *(!eol ^ _) ^ ~eol;

  }
}

#endif
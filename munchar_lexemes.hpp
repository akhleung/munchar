#ifndef MUNCHAR_LEXEMES
#define MUNCHAR_LEXEMES

#include "munchar.hpp"
#include <cctype>

namespace Munchar {
  namespace Lexemes {

    MUNCHAR_CHARACTER( null,            '\0'   );
    MUNCHAR_CHARACTER( tab,             '\t'   );
    MUNCHAR_CHARACTER( newline,         '\n'   );
    constexpr auto     linefeed       = newline;
    MUNCHAR_CHARACTER( carriage_return, '\r'   );
    MUNCHAR_KEYWORD  ( crlf,            "\r\n" );

    MUNCHAR_CHARACTER( space,           ' '  );
    MUNCHAR_CHARACTER( exclamation,     '!'  );
    MUNCHAR_CHARACTER( d_quote,         '"'  );
    constexpr auto     quote          = d_quote;
    MUNCHAR_CHARACTER( pound,           '#'  );
    constexpr auto     hash           = pound;
    constexpr auto     octothorpe     = pound;
    MUNCHAR_CHARACTER( dollar,          '$'  );
    MUNCHAR_CHARACTER( percent,         '%'  );
    MUNCHAR_CHARACTER( ampersand,       '&'  );
    MUNCHAR_CHARACTER( apostrophe,      '\'' );
    constexpr auto     s_quote        = apostrophe;
    MUNCHAR_CHARACTER( l_paren,         '(' );
    MUNCHAR_CHARACTER( r_paren,         ')' );
    MUNCHAR_CHARACTER( asterisk,        '*'  );
    MUNCHAR_CHARACTER( plus,            '+'  );
    MUNCHAR_CHARACTER( comma,           ','  );
    MUNCHAR_CHARACTER( hyphen,          '-'  );
    constexpr auto     minus          = hyphen;
    MUNCHAR_CHARACTER( period,          '.'  );
    constexpr auto     dot            = period;
    MUNCHAR_CHARACTER( slash,           '/'  );
    constexpr auto     divide         = slash;
    MUNCHAR_CHARACTER( colon,           ':'  );
    MUNCHAR_CHARACTER( semicolon,       ';'  );
    MUNCHAR_CHARACTER( less_than,       '<'  );
    MUNCHAR_CHARACTER( equals,          '='  );
    MUNCHAR_CHARACTER( greater_than,    '>'  );
    MUNCHAR_CHARACTER( question,        '?'  );
    MUNCHAR_CHARACTER( at,              '@'  );
    MUNCHAR_CHARACTER( l_bracket,       '['  );
    MUNCHAR_CHARACTER( backslash,       '\\' );
    MUNCHAR_CHARACTER( r_bracket,       ']'  );
    MUNCHAR_CHARACTER( caret,           '^'  );
    constexpr auto     circumflex     = caret;
    MUNCHAR_CHARACTER( underscore,      '_'  );
    MUNCHAR_CHARACTER( backquote,       '`'  );
    MUNCHAR_CHARACTER( tilde,           '~'  );

    constexpr auto     _              = Any { };
    MUNCHAR_PREDICATE( letter,          std::isalpha  );
    MUNCHAR_PREDICATE( alphanumeric,    std::isalnum  );
    MUNCHAR_PREDICATE( digit,           std::isdigit  );
    MUNCHAR_PREDICATE( hex_digit,       std::isxdigit );
    MUNCHAR_PREDICATE( whitespace,      std::isspace  );
    MUNCHAR_CLASS    ( sign,            "+-"          );
    constexpr auto thing = plus & _;
    // constexpr auto id_start   = letter | underscore;
    // constexpr auto id_body    = alphanumeric | underscore;
    // constexpr auto identifier = id_start & *id_body;
    // constexpr auto number     = ~sign & ((*digit & '.'_lit & +digit) | +digit);
  }
}

#endif
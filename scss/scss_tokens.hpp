#ifndef SASS_TOKENS
#define SASS_TOKENS

#include "../include/munchar.hpp"
#include "../include/munchar_tokens.hpp"
#include <cctype>

namespace Sass {
  namespace Tokens {
    using namespace Munchar;
    using Munchar::Tokens::_;

    constexpr auto h        = MUNCHAR_STATIC_PREDICATE(::isxdigit);
    constexpr auto nl       = CHR('\n') | STR("\r\n") | CHR('\r') | CHR('\f');
    constexpr auto unicode  = CHR('\\') ^ between(1,6,h) ^ ~CLS(" \t\r\n\f");

    namespace Util {
      inline constexpr bool isnonascii(unsigned char c) {
        return c > 127;
      }
    }

    constexpr auto nonascii = MUNCHAR_STATIC_PREDICATE(Util::isnonascii);
    constexpr auto escape   = unicode | Munchar::Tokens::escape_seq;
    constexpr auto nmstart  = Munchar::Tokens::id_start | nonascii | escape;
    constexpr auto nmchar   = Munchar::Tokens::alphanumeric |
                              CLS("_-") |
                              nonascii |
                              escape;
    constexpr auto string1  = Munchar::Tokens::dq_string;
    constexpr auto string2  = Munchar::Tokens::sq_string;

    constexpr auto ident    = ~CHR('-') ^ nmstart ^ *nmchar;
    constexpr auto name     = +nmchar;
    constexpr auto num      = (*Munchar::Tokens::digit ^ CHR('.') ^ +Munchar::Tokens::digit) |
                              +Munchar::Tokens::digit;
    constexpr auto string   = string1 | string2;
    constexpr auto w        = *CLS(" \t\r\n\f");
    constexpr auto variable = CHR('$') ^ ident;

    constexpr auto range = between(1,6,h|CHR('?'));

    constexpr auto s = +CLS(" \t\r\n\f");

    constexpr auto comment             = Munchar::Tokens::c_comment;
    constexpr auto single_line_comment = Munchar::Tokens::cpp_comment ^ *(w ^ Munchar::Tokens::cpp_comment);

    constexpr auto cdo            = STR("<!--");
    constexpr auto cdc            = STR("-->");
    constexpr auto includes       = STR("~=");
    constexpr auto dashmatch      = STR("|=");
    constexpr auto prefixmatch    = STR("^=");
    constexpr auto suffixmatch    = STR("$=");
    constexpr auto substringmatch = STR("*=");

    constexpr auto hash = CHR('#') ^ name;

    constexpr auto important = CHR('!') ^ w ^ STR("important");

    constexpr auto number = num ^ ~(ident | CHR('%'));

    namespace Util {
      // using (more or less) the algorithm in the CSS 3 syntax spec:
      // http://www.w3.org/TR/css3-syntax/#consume-a-url-token
      // inline const char* urlchars(const char* src) {
      //   const char* pos = src;
      //   pos = w(pos);
      //   if (*pos == '"' || *pos == '\'') {
      //     // let the parser handle the rparen
      //     return string(pos);
      //   }
      //   while (*pos != ')' && *pos != '\0') {
      //     if (Munchar::Tokens::ws_char(pos)) {
      //       ++pos;
      //     }
      //     else if (*pos == '\\') {
      //       pos = escape(pos);
      //       if (!pos) return 0; // invalid escape sequence
      //     }
      //     else if (*pos == '"' || *pos == '\'' || *pos == '(') {
      //       return 0;
      //     }
      //     else {
      //       ++pos;
      //     }
      //   }
      //   return pos;
      // }

      inline const char* urlchars(const char* src, const char* end = nullptr) {
        const char* pos = src;
        pos = w(pos, end);
        if (*pos == '"' || *pos == '\'') {
          // let the parser handle the rparen
          return end ? string(pos, end) : string(pos);
        }
        while ((end ? pos < end : *pos != '\0') && *pos != ')') {
          if (Munchar::Tokens::ws_char(pos, end)) {
            ++pos;
          }
          else if (*pos == '\\') {
            pos = end ? escape(pos, end) : escape(pos);
            if (!pos) return 0; // invalid escape sequence
          }
          else if (*pos == '"' || *pos == '\'' || *pos == '(') {
            return 0;
          }
          else {
            ++pos;
          }
        }
        return pos;
      }

    }

    constexpr auto url = MUNCHAR_STATIC_FUNCTION(Util::urlchars);
    constexpr auto uri = STR("url(") ^ url ^ CHR(')');
    constexpr auto function = ident ^ CHR('(');

    constexpr auto unicode_range = STR("u+") ^
                                   ((between(1,6,h) ^ CHR('-') ^
                                     between(1,6,h)) |
                                    range);

    constexpr auto plus    = w ^ CHR('+');
    constexpr auto greater = w ^ CHR('>');
    constexpr auto tilde   = w ^ CHR('~');
    constexpr auto css_not = STR(":not(");

    constexpr auto url_prefix = STR("url-prefix(") ^ w ^
                                (string | url) ^ w ^
                                CHR(')');
    constexpr auto domain     = STR("domain(") ^ w ^
                                (string | url) ^ w ^
                                CHR(')');

    constexpr auto hex_color    = CHR('#') ^ +h;
    constexpr auto interp_start = STR("#{");
    constexpr auto any          = CHR(':') ^
                                  ~(CHR('-') ^ name ^ CHR('-')) ^
                                  STR("any(");
    constexpr auto optional     = CHR('!') ^ w ^ STR("optional");

    constexpr auto ident_hyphen_interp = STR("-#{");
    constexpr auto string1_no_interp   = CHR('"') ^
                                         *((!CLS("\n\r\f\\\"#") ^ _) |
                                           (CHR('#') ^ !CHR('{')) |
                                           (CHR('\\') ^ nl) |
                                           escape) ^
                                         CHR('"');
    constexpr auto string2_no_interp   = CHR('\'') ^
                                         *((!CLS("\n\r\f\\'#") ^ _) |
                                           (CHR('#') ^ !CHR('{')) |
                                           (CHR('\\') ^ nl) |
                                           escape) ^
                                         CHR('\'');
    constexpr auto string_no_interp    = string1_no_interp | string2_no_interp;

    constexpr auto static_component = ident | string_no_interp | hex_color |
                                      (~CLS("+-") ^ number) | important;
    constexpr auto static_value = static_component ^
                                  *(w^(w|CLS(",/"))^w^static_component) ^
                                  CLS(";}");
    constexpr auto static_selector = between(
                                       1,50,
                                       nmchar |
                                       CLS(" \t") |
                                       CLS(",>+*") |
                                       (CLS(":#.") ^ nmstart)
                                     ) ^ CHR('{');

  }
}

#endif
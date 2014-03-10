#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "../include/munchar.hpp"
#include "../include/munchar_lexemes.hpp"

using namespace std;
using namespace Munchar;
using namespace Munchar::Lexemes;

constexpr auto import_kwd    = "@import"_lit ^ !id_body;
constexpr auto optional_kwd  = "@optional"_lit ^ !id_body;
constexpr auto func_kwd      = "@func"_lit ^ !id_body;
constexpr auto namespace_kwd = "@namespace"_lit ^ !id_body;
constexpr auto open_kwd      = "@open"_lit ^ !id_body;

constexpr auto read_kwd      = "read"_lit ^ !(id_body | colon);
constexpr auto position_kwd  = ("top"_lit | "bottom"_lit | "before"_lit | "after"_lit) ^ !(id_body | colon);

constexpr auto ts_identifier = +'$'_lit | (id_start ^ *(id_body | '$'_lit));
constexpr auto gvar          = '$'_lit ^ +id_body;
constexpr auto lvar          = '%'_lit ^ +id_body;

using Positions = vector<size_t>;

Positions lparens;
Positions rparens;
Positions lbraces;
Positions rbraces;
Positions commas;
Positions dots;
Positions gvars;
Positions lvars;
Positions identifiers;
Positions attributes;
Positions imports;
Positions optionals;
Positions funcs;
Positions namespaces;
Positions opens;


int main() {

  stringstream ss;
  for (char c = 0; (c = getchar()) != EOF; ss << c) ;

  auto src = ss.str().c_str();
  auto start = src;
  while (*src) {
    switch (*src) {

      case ' ':
      case '\t':
      case '\n':
      case '\r': {
        src = whitespace(src);
      } break;

      case '/': {
        if      (src = c_comment(src))    blah;
        else if (src = cpp_comment(src))  blah;
        else if (src = slash_regexp(src)) blah;
        else                              error;
      } break;

      case '#': {
        src = sh_comment(src);
      } break;

      case '@': {
        if      (src = import_kwd(src))    blah;
        else if (src = optional_kwd(src))  blah;
        else if (src = func_kwd(src))      blah;
        else if (src = namespace_kwd(src)) blah;
        else if (src = open_kwd(src))      blah;
        else                               error;
      } break;

      case '+': {
        ++src;
      } break;

      case '"':
      case '\'': {
        if (src = Lexemes::string(src)) blah;
        else                            error;
      } break;

      case '`': {
        if (src = backquote_regexp(src)) blah;
        else                             error;
      } break;

      case '(': {
        ++src;
      } break;

      case ')': {
        ++src;
      } break;

      case '{': {
        ++src;
      } break;

      case '}': {
        ++src;
      } break;

      case '.': {
        ++src;
      } break;

      case ',': {
        ++src;
      } break;

      case '=': {
        ++src;
      } break;

      case '$': {
        if (src = gvar(src)) blah;
        else                 src = ts_identifier(src);
      } break;

      case '%': {
        if (src = lvar(src)) blah;
        else                 error;
      } break;

      default: {
        if      (src = position_kwd(src))  check_for_attr;
        else if (src = read_kwd(src))      check_for_attr;
        else if (src = ts_identifier(src)) check_for_attr;
        else if (src = attr_name(src))     blah;
        else if (src = number(src))        blah;
        else                               error;
      } break;

    }




  }

  return 0;

}
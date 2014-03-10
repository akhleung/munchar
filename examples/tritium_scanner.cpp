#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>

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
constexpr auto position_kwd  =
("top"_lit | "bottom"_lit | "before"_lit | "after"_lit) ^ !(id_body | colon);

constexpr auto ts_identifier = +'$'_lit | (id_start ^ *(id_body | '$'_lit));
constexpr auto attr_name     = (id_start | colon) ^ *(id_body | "-:."_cls);
constexpr auto type_name     = P(::isupper) ^ *id_body;
constexpr auto gvar          = '$'_lit ^ +id_body;
constexpr auto lvar          = '%'_lit ^ +id_body;
constexpr auto ts_path       = +(id_body | "-+.*?:\\/"_cls);
constexpr auto slash_regexp  = slash ^ (escape_seq | (!"/\\"_cls ^ _)) ^ slash ^ *"imxouesn"_cls;
constexpr auto bq_regexp     = backquote ^ (escape_seq | (!"`\\"_cls ^ _)) ^ backquote ^ *"imxouesn"_cls;

enum class Tritium_Token {
  lparen, rparen, lbrace, rbrace,
  comma, dot, equal, plus,
  string, regexp, pos,
  gvar, lvar,
  kwd, id, type, path,
  ns, open, func, import, optional,
  read,
  comment
};

struct Lexeme {
  Tritium_Token t;
  const char* b;
  const char* e;
  constexpr Lexeme(Tritium_Token tok, const char* beg, const char* end)
  : t(tok), b(beg), e(end) { }
};

vector<Lexeme> lexemes;

int main() {

  stringstream ss;
  for (char c = 0; (c = getchar()) != EOF; ss << c) ;

  auto src = ss.str().c_str();
  auto pos = src;
  while (*src) {
    switch (*src) {
      case ' ':
      case '\t':
      case '\n':
      case '\r': {
        pos = whitespace(src);
      } break;

      case '/': {
        if (((pos = c_comment(src))) || ((pos = cpp_comment(src)))) {
          lexemes.push_back(Lexeme(Tritium_Token::comment, src, pos));
        }
        else if ((pos = slash_regexp(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::regexp, src, pos));
        }
        else {
          throw "malformed comment or regular expression";
        }
      } break;

      case '#': {
        pos = sh_comment(src);
        lexemes.push_back(Lexeme(Tritium_Token::comment, src, pos));
      } break;

      case '@': {
        if ((pos = import_kwd(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::import, src, pos));
        }
        else if ((pos = optional_kwd(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::optional, src, pos));
        }
        else if ((pos = func_kwd(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::func, src, pos));
        }
        else if ((pos = namespace_kwd(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::ns, src, pos));
        }
        else if ((pos = open_kwd(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::open, src, pos));
        }
        else {
          throw "unrecognized directive";
        }
      } break;

      case '+': {
        pos = src+1;
        lexemes.push_back(Lexeme(Tritium_Token::plus, src, pos));
      } break;

      case '"':
      case '\'': {
        if ((pos = Lexemes::string(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::string, src, pos));
        }
        else {
          throw "unterminated string";
        }
      } break;

      case '`': {
        if ((pos = bq_regexp(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::regexp, src, pos));
        }
        else {
          throw "malformed regular expression";
        }
      } break;

      case '(': {
        pos = src+1;
        lexemes.push_back(Lexeme(Tritium_Token::lparen, src, pos));
      } break;

      case ')': {
        pos = src+1;
        lexemes.push_back(Lexeme(Tritium_Token::rparen, src, pos));
      } break;

      case '{': {
        pos = src+1;
        lexemes.push_back(Lexeme(Tritium_Token::lbrace, src, pos));
      } break;

      case '}': {
        pos = src+1;
        lexemes.push_back(Lexeme(Tritium_Token::rbrace, src, pos));
      } break;

      case '.': {
        pos = src+1;
        lexemes.push_back(Lexeme(Tritium_Token::dot, src, pos));
      } break;

      case ',': {
        pos = src+1;
        lexemes.push_back(Lexeme(Tritium_Token::comma, src, pos));
      } break;

      case '=': {
        pos = src+1;
        lexemes.push_back(Lexeme(Tritium_Token::equal, src, pos));
      } break;

      case '$': {
        if ((pos = gvar(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::gvar, src, pos));
        }
        else {
          pos = ts_identifier(src);
          lexemes.push_back(Lexeme(Tritium_Token::id, src, pos));
        }
      } break;

      case '%': {
        if ((pos = lvar(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::lvar, src, pos));
        }
        else {
          throw "malformed local variable name";
        }
      } break;

      default: {
        if ((pos = position_kwd(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::pos, src, pos));
        }
        else if ((pos = type_name(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::type, src, pos));
        }
        else if ((pos = read_kwd(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::read, src, pos));
        }
        else if ((pos = attr_name(src))) {
          if (*(pos-1) != ':') throw "attribute name must be followed by `:`";
          lexemes.push_back(Lexeme(Tritium_Token::kwd, src, pos));
        }
        else if ((pos = ts_identifier(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::id, src, pos));
        }
        else if ((pos = number(src))) {
          lexemes.push_back(Lexeme(Tritium_Token::string, src, pos));
        }
        else {
          throw "unrecognized lexeme";
        }
      } break;
    }
    src = pos;
  }

  return 0;

}
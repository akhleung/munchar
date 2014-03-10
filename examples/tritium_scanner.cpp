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
constexpr auto attr_name     = (id_start | colon) ^ *(id_body | "-."_cls) ^ colon;
constexpr auto type_name     = P(::isupper) ^ *id_body;
constexpr auto gvar          = '$'_lit ^ +id_body;
constexpr auto lvar          = '%'_lit ^ +id_body;
constexpr auto ts_path       = +(id_body | "-+.*?:\\/"_cls);
constexpr auto slash_regexp  = slash ^ *(escape_seq | (!"/\\"_cls ^ _)) ^ slash ^ *"imxouesn"_cls;
constexpr auto bq_regexp     = backquote ^ *(escape_seq | (!"`\\"_cls ^ _)) ^ backquote ^ *"imxouesn"_cls;

enum Tritium_Token {
  LPAREN, RPAREN, LBRACE, RBRACE,
  COMMA, DOT, EQUAL, PLUS,
  STRING, REGEXP, POS,
  GVAR, LVAR,
  KWD, ID, TYPE, PATH,
  NS, OPEN, FUNC, IMPORT, OPTIONAL,
  READ,
  COMMENT
};

const char* token_names[] = {
  "LPAREN", "RPAREN", "LBRACE", "RBRACE",
  "COMMA", "DOT", "EQUAL", "PLUS",
  "STRING", "REGEXP", "POS",
  "GVAR", "LVAR",
  "KWD", "ID", "TYPE", "PATH",
  "NS", "OPEN", "FUNC", "IMPORT", "OPTIONAL",
  "READ",
  "COMMENT"
};

struct Lexeme {
  Tritium_Token t;
  const char* b;
  const char* e;
  constexpr Lexeme(Tritium_Token tok, const char* beg, const char* end)
  : t(tok), b(beg), e(end) { }
  std::string to_string() const {
    std::string rep = "[";
    rep += token_names[t];
    rep += ", ";
    rep += std::string(b, e-b);
    rep += "]";
    return rep;
  }
};

vector<Lexeme> lexemes;

int main() {

  stringstream ss;
  for (char c = 0; (c = getchar()) != EOF; ss << c) ;

  auto src_str = ss.str();
  auto src = src_str.c_str();
  const char* pos = src;
  try {
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
            // do nothing; just skip the comment
          }
          else if ((pos = slash_regexp(src))) {
            lexemes.push_back(Lexeme(REGEXP, src, pos));
          }
          else {
            throw "malformed comment or regular expression";
          }
        } break;

        case '#': {
          pos = sh_comment(src);
          // just skip the comment
        } break;

        case '@': {
          if ((pos = import_kwd(src))) {
            lexemes.push_back(Lexeme(IMPORT, src, pos));
          }
          else if ((pos = optional_kwd(src))) {
            lexemes.push_back(Lexeme(OPTIONAL, src, pos));
          }
          else if ((pos = func_kwd(src))) {
            lexemes.push_back(Lexeme(FUNC, src, pos));
          }
          else if ((pos = namespace_kwd(src))) {
            lexemes.push_back(Lexeme(NS, src, pos));
          }
          else if ((pos = open_kwd(src))) {
            lexemes.push_back(Lexeme(OPEN, src, pos));
          }
          else {
            throw "unrecognized directive";
          }
        } break;

        case '+': {
          pos = src+1;
          lexemes.push_back(Lexeme(PLUS, src, pos));
        } break;

        case '"':
        case '\'': {
          if ((pos = Lexemes::string(src))) {
            lexemes.push_back(Lexeme(STRING, src, pos));
          }
          else {
            throw "unterminated string";
          }
        } break;

        case '`': {
          if ((pos = bq_regexp(src))) {
            lexemes.push_back(Lexeme(REGEXP, src, pos));
          }
          else {
            throw "malformed regular expression";
          }
        } break;

        case '(': {
          pos = src+1;
          lexemes.push_back(Lexeme(LPAREN, src, pos));
        } break;

        case ')': {
          pos = src+1;
          lexemes.push_back(Lexeme(RPAREN, src, pos));
        } break;

        case '{': {
          pos = src+1;
          lexemes.push_back(Lexeme(LBRACE, src, pos));
        } break;

        case '}': {
          pos = src+1;
          lexemes.push_back(Lexeme(RBRACE, src, pos));
        } break;

        case '.': {
          pos = src+1;
          lexemes.push_back(Lexeme(DOT, src, pos));
        } break;

        case ',': {
          pos = src+1;
          lexemes.push_back(Lexeme(COMMA, src, pos));
        } break;

        case '=': {
          pos = src+1;
          lexemes.push_back(Lexeme(EQUAL, src, pos));
        } break;

        case '$': {
          if ((pos = gvar(src))) {
            lexemes.push_back(Lexeme(GVAR, src, pos));
          }
          else {
            pos = ts_identifier(src);
            lexemes.push_back(Lexeme(ID, src, pos));
          }
        } break;

        case '%': {
          if ((pos = lvar(src))) {
            lexemes.push_back(Lexeme(LVAR, src, pos));
          }
          else {
            throw "malformed local variable name";
          }
        } break;

        default: {
          if ((pos = attr_name(src))) {
            lexemes.push_back(Lexeme(KWD, src, pos));
          }
          else if ((pos = position_kwd(src))) {
            lexemes.push_back(Lexeme(POS, src, pos));
          }
          else if ((pos = type_name(src))) {
            lexemes.push_back(Lexeme(TYPE, src, pos));
          }
          else if ((pos = read_kwd(src))) {
            lexemes.push_back(Lexeme(READ, src, pos));
          }
          else if ((pos = ts_identifier(src))) {
            lexemes.push_back(Lexeme(ID, src, pos));
          }
          else if ((pos = number(src))) {
            lexemes.push_back(Lexeme(STRING, src, pos));
          }
          else {
            cout << std::string(src, src+10) << endl;
            throw "unrecognized lexeme";
          }
        } break;
      }
      src = pos;
    }
  }
  catch (const char* msg) {
    cerr << "error: " << msg << endl;
    return 1;
  }

  for (auto &lexeme : lexemes) {
    cout << lexeme.to_string() << endl;
  }

  return 0;

}
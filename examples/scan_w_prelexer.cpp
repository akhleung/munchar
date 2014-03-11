#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <chrono>

#include "prelexer/constants.hpp"
#include "prelexer/prelexer.hpp"

using namespace std;
using namespace Sass;
using namespace Sass::Constants;
using namespace Sass::Prelexer;

namespace Sass {
  namespace Prelexer {
    inline const char* id_start(const char* src) {
      return alternatives<alpha, exactly<'_'> >(src);
    }

    inline const char* id_body(const char* src) {
      return alternatives<alnum, exactly<'_'> >(src);
    }

    inline const char* colon(const char* src) {
      return exactly<':'>(src);
    }

    constexpr char import_k[] = "@import";
    inline const char* import_kwd(const char* src) {
      return
      sequence<
        exactly<import_k>,
        negate<id_body>
      >(src);
    }

    constexpr char optional_k[] = "@optional";
    inline const char* optional_kwd(const char* src) {
      return sequence<exactly<optional_k>, negate<id_body> >(src);
    }

    constexpr char func_k[] = "@func";
    inline const char* func_kwd(const char* src) {
      return sequence<exactly<func_k>, negate<id_body> >(src);
    }

    constexpr char namespace_k[] = "@namespace";
    inline const char* namespace_kwd(const char* src) {
      return sequence<exactly<namespace_k>, negate<id_body> >(src);
    }

    constexpr char open_k[] = "@open";
    inline const char* open_kwd(const char* src) {
      return sequence<exactly<open_k>, negate<id_body> >(src);
    }

    constexpr char read_k[] = "read";
    inline const char* read_kwd(const char* src) {
      return sequence<exactly<read_k>, negate<alternatives<id_body, colon > > >(src);
    }

    constexpr char top_k[] = "top";
    constexpr char bottom_k[] = "bottom";
    constexpr char before_k[] = "before";
    constexpr char after_k[] = "after";
    inline const char* position_kwd(const char* src) {
      return
      sequence<
        alternatives<exactly<top_k>, exactly<bottom_k>, exactly<before_k>, exactly<after_k> >,
        negate<alternatives<id_body, colon > >
      >(src);
    }

    inline const char* ts_identifier(const char* src) {
      return
      alternatives<
        one_plus<exactly<'$'> >,
        sequence<id_start, zero_plus<alternatives<id_body, exactly<'$'> > > >
      >(src);
    }

    constexpr char hyphen_dot[] = "-.";
    inline const char* attr_name(const char* src) {
      return
      sequence<
        alternatives<id_start, colon>,
        zero_plus<alternatives<id_body, class_char<hyphen_dot> > >,
        colon
      >(src);
    }

    inline const char* upper(const char* src) {
      return ::isupper(*src) ? src+1 : 0;
    }

    inline const char* type_name(const char* src) {
      return sequence<upper, zero_plus<id_body> >(src);
    }

    inline const char* gvar(const char* src) {
      return sequence<exactly<'$'>, one_plus<id_body> >(src);
    }

    inline const char* lvar(const char* src) {
      return sequence<exactly<'%'>, one_plus<id_body> >(src);
    }

    constexpr char path_chars[] = "-+.*?:\\/";
    constexpr char rgx_mod[] = "imxouesn";
    constexpr char slash_chars[] = "/\\";
    constexpr char bq_and_slash[] = "`\\";

    inline const char* ts_path(const char* src) {
      return one_plus<alternatives<id_body, class_char<path_chars> > >(src);
    }

    inline const char* escape_seq(const char* src) {
      return sequence<exactly<'\\'>, any_char>(src);
    }

    inline const char* slash_regexp(const char* src) {
      return
      sequence<
        exactly<'/'>,
        zero_plus<
          alternatives<
            escape_seq,
            sequence<
              negate<class_char<slash_chars> >,
              any_char
            >
          >
        >,
        exactly<'/'>,
        zero_plus<class_char<rgx_mod> >
      >(src);
    }

    inline const char* bq_regexp(const char* src) {
      return
      sequence<
        exactly<'`'>,
        zero_plus<
          alternatives<
            escape_seq,
            sequence<
              negate<class_char<bq_and_slash> >,
              any_char
            >
          >
        >,
        exactly<'`'>,
        zero_plus<class_char<rgx_mod> >
      >(src);
    }

    inline const char* whitespace(const char* src) {
      return optional<spaces>(src);
    }

    inline const char* c_comment(const char* src) {
      return line_comment(src);
    }

    inline const char* cpp_comment(const char* src) {
      return block_comment(src);
    }

    constexpr char crlf_chars[] = "\r\n";
    inline const char* eol(const char* src) {
      return alternatives<exactly<'\n'>, exactly<crlf_chars> >(src);
    }

    inline const char* sh_comment(const char* src) {
      return
      sequence<
        exactly<'#'>,
        zero_plus<sequence<negate<eol>, any_char> >,
        optional<eol>
      >(src);
    }
  }
}

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

  lexemes.reserve(150000);
  stringstream ss, timing_msg;
  for (char c = 0; (c = getchar()) != EOF; ss << c) ;

  std::string src_str = ss.str();
  const char* src = src_str.c_str();
  const char* pos = src;
  chrono::high_resolution_clock::time_point total;
  try {
    auto t0 = chrono::high_resolution_clock::now();
    while (*src) {
      switch (*src) {
        case ';': {
          pos = src+1;
        } break;

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
          if ((pos = Prelexer::import_kwd(src))) {
            lexemes.push_back(Lexeme(IMPORT, src, pos));
          }
          else if ((pos = Prelexer::optional_kwd(src))) {
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
          if ((pos = string_constant(src))) {
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
    auto t1 = chrono::high_resolution_clock::now();
    timing_msg << "time to tokenize: " << chrono::duration_cast<chrono::microseconds>(t1-t0).count() << " usec" << endl;
  }
  catch (const char* msg) {
    cerr << "error: " << msg << endl;
    return 1;
  }

  for (auto &lexeme : lexemes) {
    cout << lexeme.to_string() << endl;
  }

  cerr << endl << timing_msg.str();

  return 0;

}
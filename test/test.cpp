#include <cctype>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "../include/munchar.hpp"
#include "../include/munchar_tokens.hpp"

using namespace Munchar;
using namespace Munchar::Lexemes;

size_t TEST_NUM = 0;
size_t COUNT = 0;
std::vector<std::string> errors;

template<typename T>
void pass(const T& t, const char* input, const char* result) {
  ++TEST_NUM;
  const char* after = t(input);
  if (!after || std::string(input, after-input) != std::string(result)) {
    std::stringstream msg;
    msg << "test " << TEST_NUM << " should have passed on " << input << ", " << result << std::endl;
    errors.push_back(msg.str());
    std::cerr << "F";
    return;
  }
  after = t(input, input+strlen(input));
  if (!after || std::string(input, after-input) != std::string(result)) {
    std::stringstream msg;
    msg << "test " << TEST_NUM << " should have passed on " << input << ", " << result << std::endl;
    errors.push_back(msg.str());
    std::cerr << "F";
    return;
  }
  ++COUNT;
  std::cerr << ".";
}

template<typename T>
void fail(const T& t, const char* input) {
  ++TEST_NUM;
  const char* after = t(input);
  if (after) {
    std::stringstream msg;
    msg << "test " << TEST_NUM << " should have failed on " << input << std::endl;
    errors.push_back(msg.str());
    std::cerr << "F";
    return;
  }
  after = t(input, input+strlen(input));
  if (after) {
    std::stringstream msg;
    msg << "test " << TEST_NUM << " should have failed on " << input << std::endl;
    errors.push_back(msg.str());
    std::cerr << "F";
    return;
  }
  ++COUNT;
  std::cerr << ".";
}

int main() {

  pass('a'_lit, "abc", "a");
  fail('a'_lit, "bcd");
  fail('a'_lit, "");

  pass("abc"_lit, "abcdefg", "abc");
  fail("abc"_lit, "bcdefgh");
  fail("abc"_lit, "ab");
  fail("abc"_lit, "");

  pass("abc"_cls, "abcdefg", "a");
  pass("abc"_cls, "bcdefgh", "b");
  pass("abc"_cls, "cdefghi", "c");
  fail("abc"_cls, "defghij");
  fail("abc"_cls, "");

  pass(MUNCHAR_STATIC_PREDICATE(::isdigit), "12345", "1");
  pass(P(::isdigit), "12345", "1");
  fail(MUNCHAR_STATIC_PREDICATE(::isdigit), "a2345");
  fail(P(::isdigit), "a2345");
  fail(MUNCHAR_STATIC_PREDICATE(::isdigit), "");
  fail(P(::isdigit), "");

  pass("foo"_lit ^ "bar"_lit, "foobarhux", "foobar");
  pass(P(::isalpha) ^ P(::isdigit), "a1 blah", "a1");
  pass("foo"_lit ^ "bar"_lit ^ "hux"_lit, "foobarhuxbaz", "foobarhux");
  fail("foo"_lit ^ "bar"_lit, "foobah");
  fail("foo"_lit ^ "bar"_lit, "foob");
  fail("foo"_lit ^ "bar"_lit, "boof");
  fail("foo"_lit ^ "bar"_lit, "");
  fail("foo"_lit ^ "bar"_lit ^ "hux"_lit, "foobar");
  fail("foo"_lit ^ "bar"_lit ^ "hux"_lit, "");
  fail(P(::isalpha) ^ P(::isdigit), "a");
  fail(P(::isalpha) ^ P(::isdigit), "1a");
  fail(P(::isalpha) ^ P(::isdigit), "");

  pass("foo"_lit | "bar"_lit, "foobarhux", "foo");
  pass("foo"_lit | "bar"_lit, "barfoohux", "bar");
  pass("foo"_lit | "bar"_lit | "hux"_lit, "huxleberry", "hux");
  pass(P(::isalpha) | P(::isdigit), "a1 blah", "a");
  pass(P(::isalpha) | P(::isdigit), "1a blah", "1");
  fail("foo"_lit | "bar"_lit, "bahfoo");
  fail("foo"_lit | "bar"_lit, "boofoo");
  fail("foo"_lit | "bar"_lit, "boof");
  fail("foo"_lit | "bar"_lit, "");
  fail("foo"_lit | "bar"_lit | "hux"_lit, "huckleberry");
  fail(P(::isalpha) | P(::isdigit), ".");
  fail(P(::isalpha) | P(::isdigit), "!");
  fail(P(::isalpha) | P(::isdigit), "");

  pass(~("foo"_lit | "bar"_lit), "barhux", "bar");
  pass(~("foo"_lit | "bar"_lit), "bludge", "");
  pass(~("foo"_lit | "bar"_lit), "", "");

  pass(*("foo"_lit | "bar"_lit), "foobarfoofoobarstop", "foobarfoofoobar");
  pass(*("foo"_lit | "bar"_lit), "f", "");
  pass(*("foo"_lit | "bar"_lit), "", "");

  pass(+("foo"_lit | "bar"_lit), "foobarfoofoobarstop", "foobarfoofoobar");
  fail(+("foo"_lit | "bar"_lit), "f");
  fail(+("foo"_lit | "bar"_lit), "");

  pass(P(::isalpha) == 3, "abcde", "abc");
  pass(P(::isalpha) == 3, "abc12", "abc");
  pass(P(::isalpha) == 3, "abc", "abc");
  fail(P(::isalpha) == 3, "ab");
  fail(P(::isalpha) == 3, "");

  pass(P(::isalpha) < 3, "abcde", "ab");
  pass(P(::isalpha) < 3, "a", "a");
  pass(P(::isalpha) < 3, "", "");
  pass(P(::isalpha) < 3, "a123", "a");
  pass(P(::isalpha) < 3, "123", "");

  pass(P(::isalpha) <= 3, "abcde", "abc");
  pass(P(::isalpha) <= 3, "a", "a");
  pass(P(::isalpha) <= 3, "", "");
  pass(P(::isalpha) <= 3, "a123", "a");
  pass(P(::isalpha) <= 3, "123", "");

  pass(P(::isalpha) > 3, "abcde", "abcde");
  pass(P(::isalpha) > 3, "abcde123", "abcde");
  fail(P(::isalpha) > 3, "abc123");
  fail(P(::isalpha) > 3, "abc");
  fail(P(::isalpha) > 3, "");

  pass(P(::isalpha) >= 3, "abcde", "abcde");
  pass(P(::isalpha) >= 3, "abcde123", "abcde");
  pass(P(::isalpha) >= 3, "abc123", "abc");
  pass(P(::isalpha) >= 3, "abc", "abc");
  fail(P(::isalpha) >= 3, "ab123");
  fail(P(::isalpha) >= 3, "ab");
  fail(P(::isalpha) >= 3, "");

  pass(between(3, 5, 'a'_lit), "aaaab", "aaaa");
  pass(between(5, 3, 'a'_lit), "aaaab", "aaaa");
  pass(between(3, 5, 'a'_lit), "aaaaaaab", "aaaaa");
  pass(between(5, 3, 'a'_lit), "aaaaaaab", "aaaaa");
  pass(between(3, 5, 'a'_lit), "aaab", "aaa");
  pass(between(3, 3, 'a'_lit), "aaaa", "aaa");
  fail(between(3, 5, 'a'_lit), "aab");
  fail(between(3, 3, 'a'_lit), "aa");
  fail(between(3, 5, 'a'_lit), "");

  pass(!(P(::isdigit)), "abc123", "");
  pass(!(P(::isdigit)), "", "");
  fail(!(P(::isdigit)), "123abc");

  pass(&+(P(::isdigit)), "123abc", "");
  fail(&+(P(::isdigit)), "abc");
  fail(&+(P(::isdigit)), "");

  // negative lookahead
  pass(!'a'_lit ^ *P(::isalpha), "bcd123", "bcd");
  pass(!'a'_lit ^ *P(::isalpha), "", "");
  fail(!'a'_lit ^ *P(::isalpha), "abcd123");
  fail(!'a'_lit ^ +P(::isalpha), "");

  pass(*_, "abc123_! \\'", "abc123_! \\'");
  pass(*_, "", "");
  fail(+_, "");

  pass(hash ^ ((hex_digit == 6) | (hex_digit == 3)), "#abc123, 10px", "#abc123");
  pass(hash ^ ((hex_digit == 6) | (hex_digit == 3)), "#CCC, dashed", "#CCC");
  fail(hash ^ ((hex_digit == 6) | (hex_digit == 3)), "#CC, dashed");

  pass(identifier, "my_variable_name23 = blah", "my_variable_name23");
  pass(identifier, "__hidden?", "__hidden");
  fail(identifier, "23skidoo");

  pass(whitespace, "  \t\r\n hello", "  \t\r\n ");
  pass(whitespace, "hello", "");

  pass(number, "123 + x", "123");
  pass(number, "123.456 * f(x);", "123.456");
  pass(number, "+23 hello", "+23");
  pass(number, "-1-2 = -3", "-1");
  pass(number, "-12.34deg", "-12.34");
  pass(number, "+123.456e12 and other stuff", "+123.456e12");
  pass(number, "-123.456e-3 blah", "-123.456e-3");
  pass(number, "123.456e+2~!*", "123.456e+2");
  pass(number, "-.333rad", "-.333");
  pass(number, ".2 + .8", ".2");
  pass(number, "+0.002 microns", "+0.002");
  pass(number, "123.times do stuff", "123");
  fail(number, "cloud9");

  pass(string, "\"hello \\\"world\\\" and so forth\" other", "\"hello \\\"world\\\" and so forth\"");
  pass(string, "'and here\\\'s a single quoted \"string\", heh heh\\n' bungle", "'and here\\\'s a single quoted \"string\", heh heh\\n'");
  fail(string, "\"an unterminated string");
  fail(string, "'a mis-delimited string\"");

  pass(c_comment, "// blah blah blah\nnext line", "// blah blah blah\n");
  pass(c_comment, "// on Windows\r\nnext line", "// on Windows\r\n");
  pass(c_comment, "// blah blah EOF", "// blah blah EOF");

  pass(cpp_comment, "/* block comment */ blah", "/* block comment */");
  pass(cpp_comment, "/* multiline\ncomment */ blah", "/* multiline\ncomment */");
  pass(cpp_comment, "/* nested /* comment */ extra */", "/* nested /* comment */");
  fail(cpp_comment, "/* unterminated ");

  pass(sh_comment, "# blah blah blah\nnext line", "# blah blah blah\n");
  pass(sh_comment, "# on Windows\r\nnext line", "# on Windows\r\n");
  pass(sh_comment, "# blah blah EOF", "# blah blah EOF");

  if (!errors.empty()) {
    std::cerr << std::endl << TEST_NUM - COUNT << " tests failed:" << std::endl;
    for (auto &msg : errors) std::cerr << msg;
    return 1;
  }
  std::cerr << std::endl << COUNT << " tests passed (that's all of them)" << std::endl;
  return 0;
}
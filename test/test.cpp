#include <cctype>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "../include/munchar.hpp"
#include "../include/munchar_tokens.hpp"

using namespace Munchar;
using namespace Munchar::Tokens;

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

  pass(CHR('a'), "abc", "a");
  fail(CHR('a'), "bcd");
  fail(CHR('a'), "");

  pass(STR("abc"), "abcdefg", "abc");
  fail(STR("abc"), "bcdefgh");
  fail(STR("abc"), "ab");
  fail(STR("abc"), "");

  pass(CLS("abc"), "abcdefg", "a");
  pass(CLS("abc"), "bcdefgh", "b");
  pass(CLS("abc"), "cdefghi", "c");
  fail(CLS("abc"), "defghij");
  fail(CLS("abc"), "");

  pass(MUNCHAR_STATIC_PREDICATE(::isdigit), "12345", "1");
  pass(P(::isdigit), "12345", "1");
  fail(MUNCHAR_STATIC_PREDICATE(::isdigit), "a2345");
  fail(P(::isdigit), "a2345");
  fail(MUNCHAR_STATIC_PREDICATE(::isdigit), "");
  fail(P(::isdigit), "");

  pass(STR("foo") ^ STR("bar"), "foobarhux", "foobar");
  pass(P(::isalpha) ^ P(::isdigit), "a1 blah", "a1");
  pass(STR("foo") ^ STR("bar") ^ STR("hux"), "foobarhuxbaz", "foobarhux");
  fail(STR("foo") ^ STR("bar"), "foobah");
  fail(STR("foo") ^ STR("bar"), "foob");
  fail(STR("foo") ^ STR("bar"), "boof");
  fail(STR("foo") ^ STR("bar"), "");
  fail(STR("foo") ^ STR("bar") ^ STR("hux"), "foobar");
  fail(STR("foo") ^ STR("bar") ^ STR("hux"), "");
  fail(P(::isalpha) ^ P(::isdigit), "a");
  fail(P(::isalpha) ^ P(::isdigit), "1a");
  fail(P(::isalpha) ^ P(::isdigit), "");

  pass(STR("foo") | STR("bar"), "foobarhux", "foo");
  pass(STR("foo") | STR("bar"), "barfoohux", "bar");
  pass(STR("foo") | STR("bar") | STR("hux"), "huxleberry", "hux");
  pass(P(::isalpha) | P(::isdigit), "a1 blah", "a");
  pass(P(::isalpha) | P(::isdigit), "1a blah", "1");
  fail(STR("foo") | STR("bar"), "bahfoo");
  fail(STR("foo") | STR("bar"), "boofoo");
  fail(STR("foo") | STR("bar"), "boof");
  fail(STR("foo") | STR("bar"), "");
  fail(STR("foo") | STR("bar") | STR("hux"), "huckleberry");
  fail(P(::isalpha) | P(::isdigit), ".");
  fail(P(::isalpha) | P(::isdigit), "!");
  fail(P(::isalpha) | P(::isdigit), "");

  pass(~(STR("foo") | STR("bar")), "barhux", "bar");
  pass(~(STR("foo") | STR("bar")), "bludge", "");
  pass(~(STR("foo") | STR("bar")), "", "");

  pass(*(STR("foo") | STR("bar")), "foobarfoofoobarstop", "foobarfoofoobar");
  pass(*(STR("foo") | STR("bar")), "f", "");
  pass(*(STR("foo") | STR("bar")), "", "");

  pass(+(STR("foo") | STR("bar")), "foobarfoofoobarstop", "foobarfoofoobar");
  fail(+(STR("foo") | STR("bar")), "f");
  fail(+(STR("foo") | STR("bar")), "");

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

  pass(between(3, 5, CHR('a')), "aaaab", "aaaa");
  pass(between(5, 3, CHR('a')), "aaaab", "aaaa");
  pass(between(3, 5, CHR('a')), "aaaaaaab", "aaaaa");
  pass(between(5, 3, CHR('a')), "aaaaaaab", "aaaaa");
  pass(between(3, 5, CHR('a')), "aaab", "aaa");
  pass(between(3, 3, CHR('a')), "aaaa", "aaa");
  fail(between(3, 5, CHR('a')), "aab");
  fail(between(3, 3, CHR('a')), "aa");
  fail(between(3, 5, CHR('a')), "");

  pass(!(P(::isdigit)), "abc123", "");
  pass(!(P(::isdigit)), "", "");
  fail(!(P(::isdigit)), "123abc");

  pass(&+(P(::isdigit)), "123abc", "");
  fail(&+(P(::isdigit)), "abc");
  fail(&+(P(::isdigit)), "");

  // negative lookahead
  pass(!CHR('a') ^ *P(::isalpha), "bcd123", "bcd");
  pass(!CHR('a') ^ *P(::isalpha), "", "");
  fail(!CHR('a') ^ *P(::isalpha), "abcd123");
  fail(!CHR('a') ^ +P(::isalpha), "");

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
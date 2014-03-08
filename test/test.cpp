#include <cctype>
#include <string>
#include <iostream>

#include "../include/munchar.hpp"
#include "../include/munchar_lexemes.hpp"

using namespace Munchar;
using namespace Munchar::Lexemes;

template<typename T>
void pass(const T& t, const char* input, const char* result) {
  const char* after = t(input);
  if (std::string(input, after-input) != std::string(result)) {
    throw std::string("test should have passed on ") + input + ", " + result;
  }
  std::cerr << ".";
}

template<typename T>
void fail(const T& t, const char* input) {
  const char* after = t(input);
  if (after) {
    throw std::string("test should have failed on ") + input;
  }
}

int main() {

  try {
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
  }
  catch (std::string& msg) {
    std::cerr << msg << std::endl;
    return 1;
  }
  std::cerr << std::endl << "all tests passed" << std::endl;
  return 0;
}
#include <cctype>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "../include/munchar.hpp"
#include "../include/munchar_tokens.hpp"
#include "scss_tokens.hpp"

using namespace Munchar;
using namespace Sass::Tokens;

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

  pass(string_no_interp, "'hello this is a string' blah", "'hello this is a string'");
  pass(string_no_interp, "\"hello this is a {string}\" blah", "\"hello this is a {string}\"");
  fail(string_no_interp, "'here\\'s an interpolant: #{2+2} blah'");

  pass(string, "'here\\'s an interpolant: #{2+2} blah'", "'here\\'s an interpolant: #{2+2} blah'");

  pass(uri, "url(index.html)blah blah", "url(index.html)");
  pass(uri, "url(http://www.foo.com/home/index.html)blah blah", "url(http://www.foo.com/home/index.html)");
  pass(uri, "url(Hey, here\\'s an obnoxious url; suck it up!.html) blah", "url(Hey, here\\'s an obnoxious url; suck it up!.html)");

  if (!errors.empty()) {
    std::cerr << std::endl << TEST_NUM - COUNT << " tests failed:" << std::endl;
    for (auto &msg : errors) std::cerr << msg;
    return 1;
  }
  std::cerr << std::endl << COUNT << " tests passed (that's all of them)" << std::endl;
  return 0;
}
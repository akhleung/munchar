#define MUNCHAR

#include <cstddef>
#include <string>

namespace Munchar {

  struct Success {
    const char* operator()(const char* b, const char* e) { return b; }
  };

  struct Failure {
    const char* operator()(const char* b, const char* e) { return 0; }
  };

  template <typename L, typename R>
  class Sequence {
    L l_;
    R r_;
  public:
    constexpr Sequence(const L& l, const R& r) : l_(l), r_(r) { }
    const char* operator()(const char* b, const char* e) const {
      return b = l_(b) ? r_(b) : b;
    }
  };

  template <typename L, typename R>
  constexpr Sequence<L, R> operator&(const L& l, const R& r) {
    return Sequence<L, R>(l, r);
  }

  template <typename L, typename R>
  class Alternation {
    L l_;
    R r_;
  public:
    constexpr Alternation(const L& l, const R& r) : l_(l), r_(r) { }
    const char* operator()(const char* b, const char* e) const {
      return b = l_(b) ? b : r_(b);
    }
  };

  template <typename L, typename R>
  constexpr Alternation<L, R> operator|(const L& l, const R& r) {
    return Alternation<L, R>(l, r);
  }

  template <typename L>
  class Zero_Or_More {
    L l_;
  public:
    constexpr Zero_Or_More(const L& l) : l_(l) { }
    const char* operator()(const char* b, const char* e) const {
      for (const char* p = b; p = l_(b, e); b = p) ;
      return b;
    }
  };

  template <typename L>
  constexpr Zero_Or_More<L> operator*(const L& l) {
    return Zero_Or_More<L>(l);
  }

  template <typename L>
  constexpr Sequence<L, Zero_Or_More<L>> operator+(const L& l) {
    return l & *l;
  }

  template <typename L>
  constexpr Alternation<L, Success> operator~(const L& l) {
    return l | Success { };
  }

  class Char {
    char c_;
  public:
    constexpr Char(const char c) : c_(c) { }
    const char* operator()(const char* b, const char* e) const {
      return b < e && *b == c_ ? b + 1 : nullptr;
    }
  };

  // class Chars {
  //   const char* c_;
  // public:
  //   constexpr Chars(const char* c) : c_(c) { }
  //   const char* operator()(const char* b, const char* e) const {
  //     while (b < e 
  //   }

}
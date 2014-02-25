#define MUNCHAR

#include <cstddef>

namespace Munchar {

  struct Success {
    const char* operator()(const char* b, const char* e) { return b; }
  };

  struct Failure {
    const char* operator()(const char* b, const char* e) { return 0; }
  };

  struct Any {
    const char* operator()(const char* b, const char* e) {
      return b < e ? b + 1 : nullptr;
    }
  };

  class Character {
    char c_;
  public:
    constexpr Character(const char c) : c_(c) { }
    const char* operator()(const char* b, const char* e) const {
      return b < e && *b == c_ ? b + 1 : nullptr;
    }
  };

  constexpr Character operator"" _lit(const char c) {
    return Character { c };
  }

  class Characters {
    const char* c_;
    size_t n_;
  public:
    constexpr Characters(const char* c, size_t n) : c_(c), n_(n) { }
    const char* operator()(const char* b, const char* e) const {
      for (size_t i = 0; i < n_; ++i) {
        if (!(b < e) || c_[i] != *b) return nullptr;
      }
      return b;
    }
  };

  constexpr Characters operator"" _lit(const char* c, size_t len) {
    return Characters { c, len };
  }

  class Character_Class {
    const char* c_;
    size_t n_;
  public:
    constexpr Character_Class(const char* c, size_t n) : c_(c), n_(n) { }
    const char* operator()(const char* b, const char* e) const {
      if (!(b < e)) return nullptr;
      for (size_t i = 0; i < n_; ++i) if (c_[i] == *b) return b;
      return nullptr;
    }
  };

  constexpr Character_Class operator"" _cls(const char* c, size_t len) {
    return Character_Class { c, len };
  }

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

  template <typename L>
  class Exactly_N_Times {
    L l_;
    size_t n_;
  public:
    constexpr Exactly_N_Times(const L& l, const size_t n) : l_(l), n_(n) { }
    const char* operator()(const char* b, const char* e) const {
      size_t i;
      for (i = 0; i < n_ && (b = l_(b)); ++i) ;
      return i == n_ ? b : nullptr;
    }
  };

  template <typename L>
  constexpr Exactly_N_Times<L> operator==(const L& l, const size_t n) {
    return Exactly_N_Times<L> { l, n };
  }

  template <typename L>
  constexpr auto operator>(const L& l, const size_t n)
  -> decltype(l == n & +l) {
    return l == n & +l;
  }

  template <typename L>
  constexpr auto operator>=(const L& l, const size_t n)
  -> decltype(l == n & *l) {
    return l == n & *l;
  }

  template <typename L>
  constexpr auto operator<(const L& l, const size_t n)
  -> decltype(l == n | *l) {
    return l == n-1 | *l;
  }

  template <typename L>
  constexpr auto operator<=(const L&l, const size_t n)
  -> decltype(l == n | *l) {
    return l == n | *l;
  }

  template <typename L>
  constexpr auto range(const L& l, const size_t lo, const size_t hi)
  -> decltype(l == lo & l <= hi) {
    return l == lo & l <= hi;
  }

  template <typename L>
  class Negation {
    L l_;
  public:
    constexpr Negation(L& l) : l_(l) { }
    const char* operator()(const char* b, const char* e) const {
      return l_(b) ? nullptr : b;
    }
  };

  template <typename L>
  constexpr Negation<L> operator!(const L& l) {
    return Negation<L> { l };
  }

  template <typename L, typename R>
  constexpr auto operator<<(const L& l, const R& r)
  -> decltype(!l & r | Failure { }) {
    return !l & r | Failure { };
  }

  constexpr auto if_kwd   = "if"_lit;
  constexpr auto else_kwd = "else"_lit;
  constexpr auto sign_sym = '+'_lit | '-'_lit;
  constexpr auto digits   = "0123456789"_cls;

}
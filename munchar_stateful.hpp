#define MUNCHAR

#include <cstddef>

namespace Munchar {

  struct Success {
    template<typename I>
    I operator()(I b, I e) const {
      return b;
    }
  };

  struct Failure {
    template<typename I>
    I operator()(I b, I e) const {
      return nullptr;
    }
  };

  struct Any {
    template<typename I>
    I operator()(I b, I e) {
      return b != e ? ++b : nullptr;
    }
  };

  class Character {
    char c_;
  public:
    constexpr Character(const char c) : c_(c) { }
    template<typename I>
    I operator()(I b, I e) const {
      return b != e && *b == c_ ? ++b : nullptr;
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
    template<typename I>
    I operator()(I b, I e) const {
      for (size_t i = 0; i < n_; ++i) {
        if (b == e || c_[i] != *b) return nullptr;
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
    template<typename I>
    I operator()(I b, I e) const {
      if (!(b != e)) return nullptr;
      for (size_t i = 0; i < n_; ++i) if (c_[i] == *b) return b;
      return nullptr;
    }
  };

  constexpr Character_Class operator"" _cls(const char* c, size_t len) {
    return Character_Class { c, len };
  }

  template<typename L, typename R>
  class Sequence {
    L l_;
    R r_;
  public:
    constexpr Sequence(const L& l, const R& r) : l_(l), r_(r) { }
    template<typename I>
    I operator()(I b, I e) const {
      return b = l_(b, e) ? r_(b, e) : b;
    }
  };

  template<typename L, typename R>
  constexpr Sequence<L, R> operator&(const L& l, const R& r) {
    return Sequence<L, R>(l, r);
  }

  template<typename L, typename R>
  class Alternation {
    L l_;
    R r_;
  public:
    constexpr Alternation(const L& l, const R& r) : l_(l), r_(r) { }
    template<typename I>
    I operator()(I b, I e) const {
      return b = l_(b, e) ? b : r_(b, e);
    }
  };

  template<typename L, typename R>
  constexpr Alternation<L, R> operator|(const L& l, const R& r) {
    return Alternation<L, R>(l, r);
  }

  template<typename L>
  class Zero_Or_More {
    L l_;
  public:
    constexpr Zero_Or_More(const L& l) : l_(l) { }
    template<typename I>
    I operator()(I b, I e) const {
      for (const char* p = b; (p = l_(b, e)); b = p) ;
      return b;
    }
  };

  template<typename L>
  constexpr Zero_Or_More<L> operator*(const L& l) {
    return Zero_Or_More<L>(l);
  }

  template<typename L>
  constexpr Sequence<L, Zero_Or_More<L>> operator+(const L& l) {
    return l & *l;
  }

  template<typename L>
  constexpr Alternation<L, Success> operator~(const L& l) {
    return l | Success { };
  }

  template<typename L>
  class Exactly_N_Times {
    L l_;
    size_t n_;
  public:
    constexpr Exactly_N_Times(const L& l, const size_t n) : l_(l), n_(n) { }
    template<typename I>
    I operator()(I b, I e) const {
      size_t i;
      for (i = 0; i < n_ && (b = l_(b, e)); ++i) ;
      return i == n_ ? b : nullptr;
    }
  };

  template<typename L>
  constexpr Exactly_N_Times<L> operator==(const L& l, const size_t n) {
    return Exactly_N_Times<L> { l, n };
  }

  template<typename L>
  constexpr auto operator>(const L& l, const size_t n)
  -> decltype(l == n & +l) {
    return l == n & +l;
  }

  template<typename L>
  constexpr auto operator>=(const L& l, const size_t n)
  -> decltype(l == n & *l) {
    return l == n & *l;
  }

  template<typename L>
  constexpr auto operator<(const L& l, const size_t n)
  -> decltype(l == n | *l) {
    return l == n-1 | *l;
  }

  template<typename L>
  constexpr auto operator<=(const L&l, const size_t n)
  -> decltype((l == n) | *l) {
    return (l == n) | *l;
  }

  template<typename L>
  constexpr auto between(const size_t lo, const size_t hi, const L& l)
  -> decltype((l == lo) & (l <= hi)) {
    return (l == lo) & (l <= hi);
  }

  template<size_t lo, size_t hi, typename L>
  constexpr auto between(const L& l)
  -> decltype((l == lo) & (l <= hi)) {
    return (l == lo) & (l <= hi);
  }

  template<typename L>
  class Negation {
    L l_;
  public:
    constexpr Negation(const L& l) : l_(l) { }
    template<typename I>
    I operator()(I b, I e) const {
      return l_(b, e) ? nullptr : b;
    }
  };

  template<typename L>
  constexpr Negation<L> operator!(const L& l) {
    return Negation<L> { l };
  }

  template<typename L>
  class Lookahead {
    L l_;
  public:
    constexpr Lookahead(const L& l) : l_(l) { }
    template<typename I>
    I operator()(I b, I e) const {
      return l_(b, e) ? b : nullptr;
    }
  };

  template<typename L>
  constexpr Lookahead<L> operator&(const L& l) {
    return Lookahead<L> { l };
  }

  template<typename In, typename Out>
  class Predicate {
    Out (*p_)(In);
  public:
    constexpr Predicate(Out (* const p)(In)) : p_(p) { }
    template<typename I>
    I operator()(I b, I e) const {
      return (b != e) && p_(*b) ? ++b : nullptr;
    }
  };

  // template<typename I, typename O>
  // struct Predicate_Types {
  //   template<O(*P)(I)>
  //   struct Predicate {
  //     I operator()(I b, I e) const {
  //       return (b != e) && P(b) ? ++b : nullptr;
  //     }
  //   };

  //   template<O(*P)(I)>
  //   Predicate<P> make_pred() {
  //     return Predicate<P>{ };
  //   }
  // };

  template<typename I, typename O>
  constexpr Predicate<I, O> P(O (* const p)(I)) {
    return Predicate<I, O> { p };
  }

  // using ctype_predicate = int(*)(int);

  // template<ctype_predicate P>
  // struct ctype {
  //   I operator()(I b, I e) const {
  //     return (b != e) && P(*b) ? ++b : nullptr;
  //   }
  // };


}
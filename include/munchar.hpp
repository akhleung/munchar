#ifndef MUNCHAR
#define MUNCHAR

#include <cstddef>
#include <iostream>
using namespace std;

namespace Munchar {

  // Unconditional success

  struct Success {
    const char* operator()(const char* b, const char* e) const {
      return b;
    }
    const char* operator()(const char* b) const {
      return b;
    }
  };

  // Unconditional failure

  struct Failure {
    const char* operator()(const char* b, const char* e) const {
      return nullptr;
    }
    const char* operator()(const char* b) const {
      return nullptr;
    }
  };

  // Arbitrary character

  struct Any_Char {
    const char* operator()(const char* b, const char* e) const {
      return b < e ? b+1 : nullptr;
    }
    const char* operator()(const char* b) const {
      return *b ? b+1 : nullptr;
    }
  };

  // Character constant

  class Char {
    const char c_;
  public:
    constexpr Char(const char c) : c_(c) { }
    const char* operator()(const char* b, const char* e) const {
      return b < e && *b == c_ ? b+1 : nullptr;
    }
    const char* operator()(const char* b) const {
      return *b && *b == c_ ? b+1 : nullptr;
    }
  };

  constexpr Char operator"" _lit(const char c) {
    return Char { c };
  }

  constexpr Char CHR(const char c) {
    return Char { c };
  }

  // String constant

  template<typename Ptr = const char*, bool with_len = true>
  class Str {
    Ptr s_;
    size_t len_;
  public:
    constexpr Str(const Ptr& s, size_t len) : s_(s), len_(len) { }
    const char* operator()(const char* b, const char* e) const {
      Ptr s = s_;
      for (size_t i = 0; i < len_; ++i, ++b, ++s) {
        if (!(b < e) || (*s != *b)) return nullptr;
      }
      return b;
    }
    const char* operator()(const char* b) const {
      Ptr s = s_;
      for (size_t i = 0; i < len_; ++i, ++b, ++s) {
        if (!*b || (*s != *b)) return nullptr;
      }
      return b;
    }
  };

  template<typename Ptr>
  class Str<Ptr, false> {
    Ptr s_;
  public:
    constexpr Str(const Ptr& s) : s_(s) { }
    const char* operator()(const char* b, const char* e) const {
      Ptr s = s_;
      for (Ptr s = s_; *s; ++s) if (!(b < e) || (*s != *b)) return nullptr;
      return b;
    }
    const char* operator()(const char* b) const {
      Ptr s = s_;
      for (Ptr s = s_; *s; ++s) if (!*b || (*s != *b)) return nullptr;
      return b;
    }
  };

  constexpr Str<> operator"" _lit(const char* c, size_t len) {
    return Str<> { c, len };
  }

  template<typename Ptr>
  constexpr Str<Ptr, true> STR(const Ptr& s, size_t len) {
    return Str<Ptr, true> { s, len };
  }

  template<typename Ptr>
  constexpr Str<Ptr, false> STR(const Ptr& s) {
    return Str<Ptr, false> { s };
  }

  // Character class

  template<typename Ptr = const char*, bool with_len = true>
  class Char_Class {
    Ptr s_;
    size_t len_;
  public:
    constexpr Char_Class(const Ptr& s, size_t len) : s_(s), len_(len) { }
    const char* operator()(const char* b, const char* e) const {
      if (!(b < e)) return nullptr;
      Ptr s = s_;
      for (size_t i = 0; i < len_; ++i, ++s) if (*s == *b) return b+1;
      return nullptr;
    }
    const char* operator()(const char* b) const {
      if (!*b) return nullptr;
      Ptr s = s_;
      for (size_t i = 0; i < len_; ++i, ++s) if (*s == *b) return b+1;
      return nullptr;
    }
  };

  template<typename Ptr>
  class Char_Class<Ptr, false> {
    Ptr s_;
  public:
    constexpr Char_Class(const Ptr& s) : s_(s) { }
    const char* operator()(const char* b, const char* e) const {
      if (!(b < e)) return nullptr;
      for (Ptr s = s_; *s; ++s) if (*s == *b) return b+1;
      return nullptr;
    }
    const char* operator()(const char* b) const {
      if (!*b) return nullptr;
      for (Ptr s = s_; *s; ++s) if (*s == *b) return b+1;
      return nullptr;
    }
  };

  constexpr Char_Class<> operator"" _cls(const char* c, size_t len) {
    return Char_Class<> { c, len };
  }

  template<typename Ptr>
  constexpr Char_Class<Ptr, true> CLS(const Ptr& s, size_t len) {
    return Char_Class<Ptr, true> { s, len };
  }

  template<typename Ptr>
  constexpr Char_Class<Ptr, false> CLS(const Ptr& s) {
    return Char_Class<Ptr, false> { s };
  }

  // Predicate

  // template<typename P, bool is_static = false>
  // class Predicate {
  //   P p_;
  // public:
  //   constexpr Predicate(const P& p) : p_(p) { }
  //   const char* operator()(const char* b, const char* e) const {
  //     return (b < e) && p_(*b) ? b+1 : nullptr;
  //   }
  //   const char* operator()(const char* b) const {
  //     return *b && p_(*b) ? b+1 : nullptr;
  //   }
  // };

  // template<typename I, typename O>
  // class Predicate<O(I), false> {
  //   O (*const p_)(I);
  // public:
  //   constexpr Predicate(O(p)(I)) : p_(p) { }
  //   const char* operator()(const char* b, const char* e) const {
  //     return (b < e) && p_(*b) ? b+1 : nullptr;
  //   }
  //   const char* operator()(const char* b) const {
  //     return *b && p_(*b) ? b+1 : nullptr;
  //   }
  // };

  // template<typename I, typename O>
  // class Predicate<O(I), true> {
  //   template<O(p_)(I)>
  //   struct Static {
  //     const char* operator()(const char* b, const char* e) const {
  //       return (b < e) && p_(*b) ? b+1 : nullptr;
  //     }
  //     const char* operator()(const char* b) const {
  //       return *b && p_(*b) ? b+1 : nullptr;
  //     }
  //   };
  // public:
  //   template<O(p)(I)>
  //   constexpr Static<p> instantiate() const {
  //     return Static<p> { };
  //   }
  // };

  template<typename I, typename O, bool is_static = false>
  class Predicate {
    O (*const p_)(I);
  public:
    constexpr Predicate(O(p)(I)) : p_(p) { }
    const char* operator()(const char* b, const char* e) const {
      return (b < e) && p_(*b) ? b+1 : nullptr;
    }
    const char* operator()(const char* b) const {
      return *b && p_(*b) ? b+1 : nullptr;
    }
  };

  template<typename I, typename O>
  class Predicate<I, O, true> {
    template<O(p_)(I)>
    struct Static {
      const char* operator()(const char* b, const char* e) const {
        return (b < e) && p_(*b) ? b+1 : nullptr;
      }
      const char* operator()(const char* b) const {
        return *b && p_(*b) ? b+1 : nullptr;
      }
    };
  public:
    template<O(p)(I)>
    constexpr Static<p> instantiate() const {
      return Static<p> { };
    }
  };

  // template<typename P, bool is_static = false>
  // class Predicate {
  //   P p_;
  // public:
  //   constexpr Predicate(P p) : p_(p) { }
  //   const char* operator()(const char* b, const char* e) const {
  //     return (b < e) && p_(*b) ? b+1 : nullptr;
  //   }
  //   const char* operator()(const char* b) const {
  //     return *b && p_(*b) ? b+1 : nullptr;
  //   }
  // };

  // template<typename PT>
  // constexpr Predicate<PT> P(const PT& p) {
  //   return Predicate<PT> { p };
  // }

  template<typename I, typename O>
  constexpr Predicate<I, O> P(O (p)(I)) {
    return Predicate<I, O> { p };
  }

  template<typename I, typename O>
  constexpr auto infer_predicate_signature(O (p)(I))
  -> decltype(Predicate<I, O, true> { }) {
    return Predicate<I, O, true> { };
  }

  #define MUNCHAR_STATIC_PREDICATE(p)\
  (infer_predicate_signature(p).instantiate<p>())

  // // Wrapper for functions that implement the Munchar interface

  // template<typename F>
  // class Function {
  //   F f_;
  // public:
  //   constexpr Function(const F& f) : f_(f) { }
  //   const char* operator()(const char* b, const char* e) const {
  //     return f_(b, e);
  //   }
  //   const char* operator()(const char* b) const {
  //     return f_(b);
  //   }
  // };

  // template<typename Fn>
  // constexpr Function<Fn> F(Fn f) {
  //   return Function<Fn> { f };
  // }

  // Base class for unary combinators

  template<typename M>
  class Unary {
  protected:
    const M m_;
  public:
    constexpr Unary(const M& m) : m_(m) { }
  };

  // Base class for binary combinators

  template<typename L, typename R>
  class Binary {
  protected:
    const L l_;
    const R r_;
  public:
    constexpr Binary(const L& l, const R& r) : l_(l), r_(r) { }
  };

  // Sequencing

  template<typename L, typename R>
  class Sequence : public Binary<L, R> {
  public:
    using Binary<L, R>::Binary;
    const char* operator()(const char* b, const char* e) const {
      return (b = this->l_(b, e)) ? this->r_(b, e) : nullptr;
    }
    const char* operator()(const char* b) const {
      return (b = this->l_(b)) ? this->r_(b) : nullptr;
    }
  };

  template<typename L, typename R>
  constexpr Sequence<L, R> operator^(const L& l, const R& r) {
    return Sequence<L, R> { l, r };
  }

  // Alternation

  template<typename L, typename R>
  class Alternation : public Binary<L, R> {
  public:
    using Binary<L, R>::Binary;
    const char* operator()(const char* b, const char* e) const {
      const char* p = this->l_(b);
      return p ? p : this->r_(b, e);
    }
    const char* operator()(const char* b) const {
      const char* p = this->l_(b);
      return p ? p : this->r_(b);
    }
  };

  template<typename L, typename R>
  constexpr Alternation<L, R> operator|(const L& l, const R& r) {
    return Alternation<L, R> { l, r };
  }

  // Option

  template<typename M>
  constexpr auto operator~(const M& m)
  -> decltype(m | Success { }) {
    return m | Success { };
  }

  // Repetition

  template<typename M>
  class Zero_Or_More : public Unary<M> {
  public:
    using Unary<M>::Unary;
    const char* operator()(const char* b, const char* e) const {
      for (const char* p = b; (p = this->m_(b, e)); b = p) ;
      return b;
    }
    const char* operator()(const char* b) const {
      for (const char* p = b; (p = this->m_(b)); b = p) ;
      return b;
    }
  };

  template<typename M>
  constexpr Zero_Or_More<M> operator*(const M& m) {
    return Zero_Or_More<M> { m };
  }

  template<typename M>
  constexpr auto operator+(const M& m)
  -> decltype(m ^ *m) {
    return m ^ *m;
  }

  // Bounded repetition

  template<typename M>
  class Exactly_N_Times : public Unary<M> {
    size_t n_;
  public:
    constexpr Exactly_N_Times(const M& m, size_t n) : Unary<M> { m }, n_(n) { }
    const char* operator()(const char* b, const char* e) const {
      size_t i;
      for (i = 0; i < n_ && (b = this->m_(b, e)); ++i) ;
      return i == n_ ? b : nullptr;
    }
    const char* operator()(const char* b) const {
      size_t i;
      for (i = 0; i < n_ && (b = this->m_(b)); ++i) ;
      return i == n_ ? b : nullptr;
    }
  };

  template<typename M>
  constexpr Exactly_N_Times<M> operator==(const M& m,  size_t n) {
    return Exactly_N_Times<M> { m, n };
  }

  template<typename M>
  constexpr auto operator>(const M& m,  size_t n)
  -> decltype((m == n) ^ +m) {
    return (m == n) ^ +m;
  }

  template<typename M>
  constexpr auto operator>=(const M& m,  size_t n)
  -> decltype((m == n) ^ *m) {
    return (m == n) ^ *m;
  }

  template<typename M>
  constexpr auto operator<(const M& m,  size_t n)
  -> decltype((m == n-1) | *m) {
    return (m == n-1) | *m;
  }

  template<typename M>
  constexpr auto operator<=(const M& m,  size_t n)
  -> decltype((m == n) | *m) {
    return (m == n) | *m;
  }

  template<typename M>
  constexpr auto between(size_t a, size_t b, const M& m)
  -> decltype((m == a) ^ (m <= b)) {
    return (m == a) ^ (m <= b);
  }

  // Negation

  template<typename M>
  class Negation : Unary<M> {
  public:
    using Unary<M>::Unary;
    const char* operator()(const char* b, const char* e) const {
      return this->m_(b, e) ? nullptr : b;
    }
    const char* operator()(const char* b) const {
      return this->m_(b) ? nullptr : b;
    }
  };

  template<typename M>
  constexpr Negation<M> operator!(const M& m) {
    return Negation<M> { m };
  }

  // Lookahead

  template<typename M>
  class Lookahead : Unary<M> {
  public:
    using Unary<M>::Unary;
    const char* operator()(const char* b, const char* e) const {
      return this->m_(b, e) ? b : nullptr;
    }
    const char* operator()(const char* b) const {
      return this->m_(b) ? b : nullptr;
    }
  };

  template<typename M>
  constexpr Lookahead<M> operator&(const M& m) {
    return Lookahead<M> { m };
  }

}

#endif
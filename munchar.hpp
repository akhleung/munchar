#ifndef MUNCHAR
#define MUNCHAR

#include <cstddef>

namespace Munchar {

  enum class Stateless { tag };
  enum class Stateful  { tag };

  template<typename T>
  struct Success;

  template<>
  struct Success<Stateless> {
    template<typename I>
    static I munch(I b, I e) {
      return b;
    }
  };

  template<>
  struct Success<Stateful> {
    template<typename I>
    I munch(I b, I e) const {
      return b;
    }
  };

  template<typename T>
  struct Failure;

  template<>
  struct Failure<Stateless> {
    template<typename I>
    static I munch(I b, I e) {
      return nullptr;
    }
  };

  template<>
  struct Failure<Stateful> {
    template<typename I>
    I munch(I b, I e) const {
      return nullptr;
    }
  };

  template<typename T>
  struct Any;

  template<>
  struct Any<Stateless> {
    template<typename I>
    static I munch(I b, I e) {
      return b != e ? ++b : nullptr;
    }
  };

  template<>
  struct Any<Stateful> {
    template<typename I>
    I munch(I b, I e) const {
      return b != e ? ++b : nullptr;
    }
  };

  template<typename T, const char C = 0>
  class Character;

  template<const char c_>
  class Character<Stateless, c_> {
  public:
    template<typename I>
    static I munch(I b, I e) {
      return b != e && *b == c_ ? ++b : nullptr;
    }
  };

  template<>
  class Character<Stateful> {
    const char c_;
  public:
    constexpr Character(const char c) : c_(c) { }
    template<typename I>
    I munch(I b, I e) const {
      return b != e && *b == c_ ? ++b : nullptr;
    }
  };

  constexpr Character<Stateful> operator"" _lit(const char c) {
    return Character<Stateful> { c };
  }

  template<typename T, const char* C = nullptr, const size_t L = 0>
  class C_String;

  // stateless munchar for null-terminated strings
  template<const char* c_>
  class C_String<Stateless, c_, 0> {
  public:
    template<typename I>
    static I munch(I b, I e) {
      for (const char* p = c_; *p; ++b, ++p) {
        if (b == e || *p != *b) return nullptr;
      }
      return b;
    }
  };

  // stateless munchar for strings with an explicit length (may contain nulls)
  // (will probably go unused, but this option is worth providing)
  template<const char* c_, const size_t l_>
  class C_String<Stateless, c_, l_> {
  public:
    template<typename I>
    static I munch(I b, I e) {
      for (size_t i = 0; i < l_; ++i, ++b) {
        if (b == e || c_[i] != *b) return nullptr;
      }
      return b;
    }
  };

  // stateful munchar for strings with an explicit length (may contain nulls)
  template<>
  class C_String<Stateful> {
    const char* c_;
    const size_t l_;
  public:
    constexpr C_String(const char* c, const size_t l) : c_(c), l_(l) { }
    template<typename I>
    I munch(I b, I e) const {
      for (size_t i = 0; i < l_; ++i, ++b) {
        if (b == e || c_[i] != *b) return nullptr;
      }
      return b;
    }
  };

  template<const char* c, const size_t l = 0>
  constexpr C_String<Stateless, c, l>cstr() {
    return C_String<Stateless, c, l> { };
  }

  constexpr auto operator"" _lit(const char* c, size_t len)
  -> decltype(C_String<Stateful> { c, len }) {
    return C_String<Stateful> { c, len };
  }

  template<typename T, const char* C = nullptr, const size_t L = 0>
  class Character_Class;

  // stateless munchar for null-terminated character classes
  template<const char* c_>
  class Character_Class<Stateless, c_, 0> {
  public:
    template<typename I>
    static I munch(I b, I e) {
      if (b == e) return nullptr;
      for (const char* p = c_; *p; ++p) if (*p == *b) return b;
      return nullptr;
    }
  };

  // stateless munchar for character classes with an explicit length
  // (may contain nulls; will probably go unused)
  template<const char* c_, const size_t l_>
  class Character_Class<Stateless, c_, l_> {
  public:
    template<typename I>
    static I munch(I b, I e) {
      if (b == e) return nullptr;
      for (size_t i = 0; i < l_; ++i) if (c_[i] == *b) return b;
      return nullptr;
    }
  };

  // stateful munchar for character classes with an explicit length
  template<>
  class Character_Class<Stateful> {
    const char* c_;
    size_t n_;
  public:
    constexpr Character_Class(const char* c, size_t n) : c_(c), n_(n) { }
    template<typename I>
    I munch(I b, I e) const {
      if (!(b != e)) return nullptr;
      for (size_t i = 0; i < n_; ++i) if (c_[i] == *b) return b;
      return nullptr;
    }
  };

  constexpr auto operator"" _cls(const char* c, size_t len)
  -> decltype(Character_Class<Stateful> { c, len }) {
    return Character_Class<Stateful> { c, len };
  }

  // sequencing

  template<typename T, typename L, typename R>
  class Sequence;

  template<template<typename> class L, template<typename> class R>
  class Sequence<Stateless, L<Stateless>, R<Stateless>> {
    template<typename I>
    static I munch(I b, I e) {
      return b = L<Stateless>::munch(b, e) ? R<Stateless>::munch(b, e) : b;
    }
  };

  template<template<typename> class L, template<typename> class R>
  class Sequence<Stateful, L<Stateful>, R<Stateless>> {
    const L<Stateful> l_;
  public:
    constexpr Sequence(const L<Stateful>& l) : l_(l) { }
    template<typename I>
    I munch(I b, I e) const {
      return b = l_.munch(b, e) ? R<Stateless>::munch(b, e) : b;
    }
  };

  template<template<typename> class L, template<typename> class R>
  class Sequence<Stateful, L<Stateless>, R<Stateful>> {
    const R<Stateful> r_;
  public:
    constexpr Sequence(const R<Stateful>& r) : r_(r) { }
    template<typename I>
    I munch(I b, I e) const {
      return b = L<Stateless>::munch(b, e) ? r_.munch(b, e) : b;
    }
  };

  template<template<typename> class L, template<typename> class R>
  class Sequence<Stateful, L<Stateful>, R<Stateful>> {
    const L<Stateful> l_;
    const R<Stateful> r_;
  public:
    constexpr Sequence(const L<Stateful>& l, const R<Stateful>& r)
    : l_(l), r_(r) { }
    template<typename I>
    I munch(I b, I e) const {
      return b = l_munch(b, e) ? r_.munch(b, e) : b;
    }
  };

  template<template<typename> class L, template<typename> class R>
  constexpr auto operator&(const L<Stateless>&, const R<Stateless>&)
  -> decltype(Sequence<Stateless, L<Stateless>, R<Stateless>> { }) {
    return Sequence<Stateless, L<Stateless>, R<Stateless>> { };
  }

  template<template<typename> class L, template<typename> class R>
  constexpr auto operator&(const L<Stateful>& l, const R<Stateless>)
  -> decltype(Sequence<Stateful, L<Stateful>, R<Stateless>> { l }) {
    return Sequence<Stateful, L<Stateful>, R<Stateless>> { l };
  }

  template<template<typename> class L, template<typename> class R>
  constexpr auto operator&(const L<Stateless>&, const R<Stateful> r)
  -> decltype(Sequence<Stateful, L<Stateless>, R<Stateful>> { r }) {
    return Sequence<Stateful, L<Stateless>, R<Stateful>> { r };
  }

  template<template<typename> class L, template<typename> class R>
  constexpr auto operator&(const L<Stateful>& l, const R<Stateful> r)
  -> decltype(Sequence<Stateful, L<Stateful>, R<Stateful>> { l, r }) {
    return Sequence<Stateful, L<Stateful>, R<Stateful>> { l, r };
  }

  // alternation

  template<typename T, typename L, typename R>
  class Alternation;

  template<template<typename> class L, template<typename> class R>
  class Alternation<Stateless, L<Stateless>, R<Stateless>> {
    template<typename I>
    static I munch(I b, I e) {
      return b = L<Stateless>::munch(b, e) ? b : R<Stateless>::munch(b, e);
    }
  };

  template<template<typename> class L, template<typename> class R>
  class Alternation<Stateful, L<Stateful>, R<Stateless>> {
    const L<Stateful> l_;
  public:
    constexpr Alternation(const L<Stateful>& l) : l_(l) { }
    template<typename I>
    I munch(I b, I e) const {
      return b = l_.munch(b, e) ? b : R<Stateless>::munch(b, e);
    }
  };

  template<template<typename> class L, template<typename> class R>
  class Alternation<Stateful, L<Stateless>, R<Stateful>> {
    const R<Stateful> r_;
  public:
    constexpr Alternation(const R<Stateful>& r) : r_(r) { }
    template<typename I>
    I munch(I b, I e) const {
      return b = L<Stateless>::munch(b, e) ? b : r_.munch(b, e);
    }
  };

  template<template<typename> class L, template<typename> class R>
  class Alternation<Stateful, L<Stateful>, R<Stateful>> {
    const L<Stateful> l_;
    const R<Stateful> r_;
  public:
    constexpr Alternation(const L<Stateful>& l, const R<Stateful>& r)
    : l_(l), r_(r) { }
    template<typename I>
    I munch(I b, I e) const {
      return b = l_.munch(b, e) ? b : r_.munch(b, e);
    }
  };

  template<template<typename> class L, template<typename> class R>
  constexpr auto operator|(const L<Stateless>&, const R<Stateless>&)
  -> decltype(Alternation<Stateless, L<Stateless>, R<Stateless>> { }) {
    return Alternation<Stateless, L<Stateless>, R<Stateless>> { };
  }

  template<template<typename> class L, template<typename> class R>
  constexpr auto operator|(const L<Stateful>& l, const R<Stateless>)
  -> decltype(Alternation<Stateful, L<Stateful>, R<Stateless>> { l }) {
    return Alternation<Stateful, L<Stateful>, R<Stateless>> { l };
  }

  template<template<typename> class L, template<typename> class R>
  constexpr auto operator|(const L<Stateless>&, const R<Stateful> r)
  -> decltype(Alternation<Stateful, L<Stateless>, R<Stateful>> { r }) {
    return Alternation<Stateful, L<Stateless>, R<Stateful>> { r };
  }

  template<template<typename> class L, template<typename> class R>
  constexpr auto operator|(const L<Stateful>& l, const R<Stateful> r)
  -> decltype(Alternation<Stateful, L<Stateful>, R<Stateful>> { l, r }) {
    return Alternation<Stateful, L<Stateful>, R<Stateful>> { l, r };
  }

  template<typename T, typename L>
  class Zero_Or_More;

  template<template<typename> class L>
  class Zero_Or_More<Stateless, L<Stateless>> {
  public:
    template<typename I>
    static I munch(I b, I e) {
      for (const char* p = b; (p = L<Stateless>::munch(b, e)); b = p) ;
      return b;
    }

  };

  template<template<typename> class L>
  class Zero_Or_More<Stateful, L<Stateful>> {
    const L<Stateful> l_;
  public:
    constexpr Zero_Or_More(const L<Stateful>& l) : l_(l) { }
    template<typename I>
    I munch(I b, I e) const {
      for (const char* p = b; (p = l_.munch(b, e)); b = p) ;
      return b;
    }
  };

  template<template<typename> class L>
  constexpr auto operator*(const L<Stateless>&)
  -> decltype(Zero_Or_More<Stateless, L<Stateless>> { }) {
    return Zero_Or_More<Stateless, L<Stateless>> { };
  }

  template<template<typename> class L>
  constexpr auto operator*(const L<Stateful>& l)
  -> decltype(Zero_Or_More<Stateful, L<Stateful>> { l }) {
    return Zero_Or_More<Stateful, L<Stateful>> { l };
  }

  template<typename L>
  constexpr auto operator+(const L& l)
  -> decltype(l & *l) {
    return l & *l;
  }

  template<template<typename> class L>
  constexpr auto operator~(const L<Stateless>& l)
  -> decltype(l | Success<Stateless> { }) {
    return l | Success<Stateless> { };
  }

  template<template<typename> class L>
  constexpr auto operator~(const L<Stateful>& l)
  -> decltype(l | Success<Stateful> { }) {
    return l | Success<Stateful> { };
  }


  // template<typename L>
  // constexpr Alternation<L, Success> operator~(const L& l) {
  //   return l | Success { };
  // }

  // template<typename L>
  // class Exactly_N_Times {
  //   L l_;
  //   size_t n_;
  // public:
  //   constexpr Exactly_N_Times(const L& l, const size_t n) : l_(l), n_(n) { }
  //   template<typename I>
  //   I munch(I b, I e) const {
  //     size_t i;
  //     for (i = 0; i < n_ && (b = l_(b, e)); ++i) ;
  //     return i == n_ ? b : nullptr;
  //   }
  // };

  // template<typename L>
  // constexpr Exactly_N_Times<L> operator==(const L& l, const size_t n) {
  //   return Exactly_N_Times<L> { l, n };
  // }

  // template<typename L>
  // constexpr auto operator>(const L& l, const size_t n)
  // -> decltype(l == n & +l) {
  //   return l == n & +l;
  // }

  // template<typename L>
  // constexpr auto operator>=(const L& l, const size_t n)
  // -> decltype(l == n & *l) {
  //   return l == n & *l;
  // }

  // template<typename L>
  // constexpr auto operator<(const L& l, const size_t n)
  // -> decltype(l == n | *l) {
  //   return l == n-1 | *l;
  // }

  // template<typename L>
  // constexpr auto operator<=(const L&l, const size_t n)
  // -> decltype((l == n) | *l) {
  //   return (l == n) | *l;
  // }

  // template<typename L>
  // constexpr auto between(const size_t lo, const size_t hi, const L& l)
  // -> decltype((l == lo) & (l <= hi)) {
  //   return (l == lo) & (l <= hi);
  // }

  // template<size_t lo, size_t hi, typename L>
  // constexpr auto between(const L& l)
  // -> decltype((l == lo) & (l <= hi)) {
  //   return (l == lo) & (l <= hi);
  // }

  // template<typename L>
  // class Negation {
  //   L l_;
  // public:
  //   constexpr Negation(const L& l) : l_(l) { }
  //   template<typename I>
  //   I munch(I b, I e) const {
  //     return l_(b, e) ? nullptr : b;
  //   }
  // };

  // template<typename L>
  // constexpr Negation<L> operator!(const L& l) {
  //   return Negation<L> { l };
  // }

  // template<typename L>
  // class Lookahead {
  //   L l_;
  // public:
  //   constexpr Lookahead(const L& l) : l_(l) { }
  //   template<typename I>
  //   I munch(I b, I e) const {
  //     return l_(b, e) ? b : nullptr;
  //   }
  // };

  // template<typename L>
  // constexpr Lookahead<L> operator&(const L& l) {
  //   return Lookahead<L> { l };
  // }

  // template<typename In, typename Out>
  // class Predicate {
  //   Out (*p_)(In);
  // public:
  //   constexpr Predicate(Out (* const p)(In)) : p_(p) { }
  //   template<typename I>
  //   I munch(I b, I e) const {
  //     return (b != e) && p_(*b) ? ++b : nullptr;
  //   }
  // };

  // // template<typename I, typename O>
  // // struct Predicate_typenames {
  // //   template<O(*P)(I)>
  // //   struct Predicate {
  // //     I munch(I b, I e) const {
  // //       return (b != e) && P(b) ? ++b : nullptr;
  // //     }
  // //   };

  // //   template<O(*P)(I)>
  // //   Predicate<P> make_pred() {
  // //     return Predicate<P>{ };
  // //   }
  // // };

  // template<typename I, typename O>
  // constexpr Predicate<I, O> P(O (* const p)(I)) {
  //   return Predicate<I, O> { p };
  // }

  // using ctype_predicate = int(*)(int);

  // template<ctype_predicate P>
  // struct ctype {
  //   I munch(I b, I e) const {
  //     return (b != e) && P(*b) ? ++b : nullptr;
  //   }
  // };


}

#endif
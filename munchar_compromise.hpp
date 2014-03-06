#ifndef MUNCHAR
#define MUNCHAR

#define MUNCHAR_CHARACTER(name, chr) constexpr auto name = just<chr>();

#define MUNCHAR_KEYWORD(name, kwd)\
constexpr char __MUNCHAR_CONSTANT_ ## name ## __[] = kwd;\
constexpr auto name = just<__MUNCHAR_CONSTANT_ ## name ## __>();

#define MUNCHAR_CLASS(name, cls)\
constexpr char __MUNCHAR_CONSTANT_ ## name ## __[] = cls;\
constexpr auto name = from<__MUNCHAR_CONSTANT_ ## name ## __>();

#define MUNCHAR_PREDICATE(name, pred)\
constexpr auto name = infer_types_from(pred).instantiate<pred>();

#include <cstddef>

namespace Munchar {

  enum class Stateless { tag };
  enum class Stateful  { tag };

  template<typename T>
  struct Success_;

  template<>
  struct Success_<Stateless> {
    template<typename I>
    static I munch(I b, I e) {
      return b;
    }
  };

  using Success = Success_<Stateless>;

  template<typename T>
  struct Failure_;

  template<>
  struct Failure_<Stateless> {
    template<typename I>
    static I munch(I b, I e) {
      return nullptr;
    }
  };

  using Failure = Failure_<Stateless>;

  template<typename T, typename U = Stateless>
  struct Any_;

  template<>
  struct Any_<Stateless, Stateless> {
    template<typename I>
    static I munch(I b, I e) {
      return b != e ? ++b : nullptr;
    }
  };

  using Any = Any_<Stateless>;

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

  template<const char C>
  constexpr auto just()
  -> decltype(Character<Stateless, C> { }) {
    return Character<Stateless, C> { };
  }

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
  constexpr C_String<Stateless, c, l>just() {
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

  template<const char* C>
  constexpr auto from()
  -> decltype(Character_Class<Stateless, C> { }) {
    return Character_Class<Stateless, C> { };
  }

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
  -> decltype(l | Success { }) {
    return l | Success { };
  }

  template<template<typename> class L>
  constexpr auto operator~(const L<Stateful>& l)
  -> decltype(l | Success { }) {
    return l | Success { };
  }

  template<typename T, typename L, size_t N = 0>
  class Exactly_N_Times;

  template<template<typename> class L, size_t n_>
  class Exactly_N_Times<Stateless, L<Stateless>, n_> {
  public:
    template<typename I>
    static I munch(I b, I e) {
      size_t i;
      for (i = 0; i < n_ && (b = L<Stateless>::munch(b, e)); ++i) ;
      return i == n_ ? b : nullptr;
    }
  };

  template<template<typename> class L>
  class Exactly_N_Times<Stateful, L<Stateful>> {
    const L<Stateful> l_;
    size_t n_;
  public:
    constexpr Exactly_N_Times(const L<Stateful>& l, const size_t n)
    : l_(l), n_(n) { }
    template<typename I>
    I munch(I b, I e) const {
      size_t i;
      for (i = 0; i < n_ && (b = l_.munch(b, e)); ++i) ;
      return i == n_ ? b : nullptr;
    }
  };

  template<size_t N, template<typename> class L>
  constexpr auto just(const L<Stateless>&)
  -> decltype(Exactly_N_Times<Stateless, L<Stateless>, N> { }) {
    return Exactly_N_Times<Stateless, L<Stateless>, N> { }; 
  }

  template<template<typename> class L>
  constexpr auto operator==(const L<Stateful>& l, const size_t n)
  -> decltype(Exactly_N_Times<Stateful, L<Stateful>> { l, n }) {
    return Exactly_N_Times<Stateful, L<Stateful>> { l, n };
  }

  template<size_t N, template<typename> class L>
  constexpr auto more_than(const L<Stateless>& l)
  -> decltype(just<N>(l) & +l) {
    return just<N>(l) & +l;
  }

  template<template<typename> class L>
  constexpr auto operator>(const L<Stateful>& l, const size_t n)
  -> decltype(l == n & +l) {
    return l == n & +l;
  }

  template<size_t N, template<typename> class L>
  constexpr auto at_least(const L<Stateless>& l)
  -> decltype(just<N> & *l) {
    return just<N> & *l;
  }

  template<template<typename> class L>
  constexpr auto operator>=(const L<Stateful>& l, const size_t n)
  -> decltype(l == n & *l) {
    return l == n & *l;
  }

  template<size_t N, template<typename> class L>
  constexpr auto less_than(const L<Stateless>& l)
  -> decltype(just<N-1> | *l) {
    return just<N-1> | *l;
  }

  template<template<typename> class L>
  constexpr auto operator<(const L<Stateful>& l, const size_t n)
  -> decltype(l == n | *l) {
    return l == n-1 | *l;
  }

  template<size_t N, template<typename> class L>
  constexpr auto at_most(const L<Stateless>& l)
  -> decltype(just<N> | *l) {
    return just<N> | *l;
  }

  template<template<typename> class L>
  constexpr auto operator<=(const L<Stateful>&l, const size_t n)
  -> decltype((l == n) | *l) {
    return (l == n) | *l;
  }

  template<size_t a, size_t b, template<typename> class L>
  constexpr auto between(const L<Stateless>& l)
  -> decltype(just<a>(l) & at_most<b>(l)) {
    return just<a>(l) & at_most<b>(l);
  }

  template<template<typename> class L>
  constexpr auto between(const size_t a, const size_t b, const L<Stateful>& l)
  -> decltype((l == a) & (l <= b)) {
    return (l == a) & (l <= b);
  }

  template<typename T, typename L>
  class Negation;

  template<template<typename> class L>
  class Negation<Stateless, L<Stateless>> {
  public:
    template<typename I>
    static I munch(I b, I e) {
      return L<Stateless>::munch(b, e) ? nullptr : b;
    }
  };

  template<template<typename> class L>
  class Negation<Stateful, L<Stateful>> {
    const L<Stateful> l_;
  public:
    constexpr Negation(const L<Stateful>& l) : l_(l) { }
    template<typename I>
    I munch(I b, I e) const {
      return l_.munch(b, e) ? nullptr : b;
    }
  };

  template<template<typename> class L>
  constexpr auto operator!(const L<Stateless>& l)
  -> decltype(Negation<Stateless, L<Stateless>> { }) {
    return Negation<Stateless, L<Stateless>> { };
  }

  template<template<typename> class L>
  constexpr Negation<Stateful, L<Stateful>> operator!(const L<Stateful>& l) {
    return Negation<Stateful, L<Stateful>> { l };
  }

  template<typename T, typename L>
  class Lookahead;

  template<template<typename> class L>
  class Lookahead<Stateless, L<Stateless>> {
  public:
    template<typename I>
    static I munch(I b, I e) {
      return L<Stateless>::munch(b, e) ? b : nullptr;
    }
  };

  template<template<typename> class L>
  class Lookahead<Stateful, L<Stateful>> {
    const L<Stateful> l_;
  public:
    constexpr Lookahead(const L<Stateful>& l) : l_(l) { }
    template<typename I>
    I munch(I b, I e) const {
      return l_.munch(b, e) ? b : nullptr;
    }
  };

  template<template<typename> class L>
  constexpr auto operator&(const L<Stateless>&l)
  -> decltype(Lookahead<Stateless, L<Stateless>> { }) {
    return Lookahead<Stateless, L<Stateless>> { };
  }

  template<template<typename> class L>
  constexpr Lookahead<Stateful, L<Stateful>> operator&(const L<Stateful>& l) {
    return Lookahead<Stateful, L<Stateful>> { l };
  }

  template<typename T, typename In, typename Out>
  class Predicate;

  template<typename In, typename Out>
  class Predicate<Stateless, In, Out> {
    template<typename T, Out(*P)(In) = nullptr>
    struct Function;

    template<Out(*P)(In)>
    struct Function<Stateless, P> {
      template<typename I>
      static I munch(I b, I e) {
        return (b != e) && P(b) ? ++b : nullptr;
      }
    };
  public:
    template<Out(*P)(In)>
    static constexpr Function<Stateless, P> from() {
      return Function<Stateless, P> { };
    }
    template<Out(*P)(In)>
    static constexpr Function<Stateless, P> instantiate() {
      return from<P>();
    }
  };

  template<typename In, typename Out>
  class Predicate<Stateful, In, Out> {
    Out (*p_)(In);
  public:
    constexpr Predicate(Out (* const p)(In)) : p_(p) { }
    template<typename I>
    I munch(I b, I e) const {
      return (b != e) && p_(*b) ? ++b : nullptr;
    }
  };

  template<typename I, typename O, O(*p)(I)>
  constexpr auto P()
  -> decltype(Predicate<Stateless, I, O>::template from<p>()) {
    return Predicate<Stateless, I, O>::template from<p>();
  }

  template<typename I, typename O>
  constexpr Predicate<Stateful, I, O> P(O (* const p)(I)) {
    return Predicate<Stateful, I, O> { p };
  }

  template<int(*p)(int)>
  constexpr auto ctype()
  -> decltype(Predicate<Stateless, int, int>::template from<p>()) {
    return Predicate<Stateless, int, int>::template from<p>();
  }

  template<bool(*p)(const char)>
  constexpr auto P()
  -> decltype(Predicate<Stateless, bool, const char>::template from<p>()) {
    return Predicate<Stateless, bool, const char>::template from<p>();
  }

  template<typename I, typename O>
  constexpr auto infer_types_from(O(*p)(I))
  -> decltype(Predicate<Stateless, I, O> { }) {
    return Predicate<Stateless, I, O> { };
  }

}

#endif
#ifndef MUNCHAR
#define MUNCHAR

#include <cstddef>

namespace Munchar {

  struct Success {
    static const char* munch(const char* b, const char* e) {
      return b;
    }
  };

  struct Failure {
    static const char* munch(const char* b, const char* e) {
      return nullptr;
    }
  };

  struct Any {
    static const char* munch(const char* b, const char* e) {
      return b != e ? ++b : nullptr;
    }
  };

  // template<template<is_static> class L, template<is_static> class R>
  // constexpr Sequence<L<yes>, R<no>> operator&(const L<yes>& l, const R<no>& r)
  // { ; }

  template <typename L, typename R>
  struct Sequence {
    static const char* munch(const char* b, const char* e) {
      return b = L::munch(b, e) ? R::munch(b, e) : b;
    }
  };

  template <typename L, typename R>
  constexpr Sequence<L, R> operator&(const L& l, const R& r)
  { return Sequence<L, R> { }; }

  template <typename L, typename R>
  struct Alternatation {
    static const char* munch(const char* b, const char* e)
    { return b = L::munch(b, e) ? b : R::munch(b, e); }
  };

  template <typename L, typename R>
  constexpr Alternatation<L, R> operator|(const L& l, const R& r)
  { return Alternatation<L, R> { }; }

  template <typename L>
  struct Zero_Plus {
    static const char* munch(const char* b, const char* e)
    {
      for (const char* pos = b; pos = L::munch(b, e); b = pos) ;
      return b;
    }
  };

  template <typename L>
  constexpr Zero_Plus<L> operator*(const L& l)
  { return Zero_Plus<L> { }; }

  template <typename L>
  constexpr Sequence<L, Zero_Plus<L>> operator+(const L& l)
  { return l & *l; }

  template <typename L>
  constexpr Alternatation<L, Epsilon> operator~(const L& l)
  { return l | Epsilon { }; }

  template <typename L, size_t N>
  struct Just_N_Times
  {
    static const char* munch(const char* b, const char* e)
    {
      for (size_t i = 0; i < N; ++i)
        if (!(b = L::munch(b, e))) return b;
      return b;
    }
  };

  template <size_t N, typename L>
  constexpr Just_N_Times<L, N> just(const L& l)
  { return Just_N_Times<L, N> { }; }

  template <typename L>
  constexpr Just_N_Times<L, 1> just(const L& l)
  { return just<1>(l); }

  template <const char* prefix>
  struct Just_This {
    static const char* munch(const char* b, const char* e)
    {
      const char* pos = prefix;
      while (*pos && b < e && *b == *pos) ++b, ++pos;
      return *pos ? b : nullptr;
    }
  };

  template <const char* prefix>
  constexpr Just_This<prefix> just()
  { return Just_This<prefix> { }; }

}

#endif
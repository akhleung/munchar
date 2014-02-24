#define MUNCHAR

#include <cstddef>
#include <string>

using namespace std;

namespace Munchar {

  struct Epsilon {
    static const char* munch(const char* src, const char* end)
    { return src; }
  };

  template <typename L, typename R>
  struct Sequence {
    static const char* munch(const char* src, const char* end)
    { return src = L::munch(src, end) ? R::munch(src, end) : src; }
  };

  template <typename L, typename R>
  constexpr Sequence<L, R> operator&(const L& l, const R& r)
  { return Sequence<L, R> { }; }

  template <typename L, typename R>
  struct Alternatation {
    static const char* munch(const char* src, const char* end)
    { return src = L::munch(src, end) ? src : R::munch(src, end); }
  };

  template <typename L, typename R>
  constexpr Alternatation<L, R> operator|(const L& l, const R& r)
  { return Alternatation<L, R> { }; }

  template <typename L>
  struct Zero_Plus {
    static const char* munch(const char* src, const char* end)
    {
      for (const char* pos = src; pos = L::munch(src, end); src = pos) ;
      return src;
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
    static const char* munch(const char* src, const char* end)
    {
      for (size_t i = 0; i < N; ++i)
        if (!(src = L::munch(src, end))) return src;
      return src;
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
    static const char* munch(const char* src, const char* end)
    {
      const char* pos = prefix;
      while (*pos && src < end && *src == *pos) ++src, ++pos;
      return *pos ? src : nullptr;
    }
  };

  template <const char* prefix>
  constexpr Just_This<prefix> just()
  { return Just_This<prefix> { }; }

}


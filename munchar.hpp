#define MUNCHAR

#include <cstddef>
#include <string>

namespace Munchar {

	struct Epsilon {
		static const char* munch(const char* src, const char* end)
		{ return src; }
	};

	template <typename L, typename R>
	struct Sequence {
		static const char* munch(const char* src, const char* end)
		{	return src = L::munch(src, end) ? R::munch(src, end) : src; }
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
	struct Exactly_N_Times
	{
		static const char* munch(const char* src, const char* end)
		{
			for (size_t i = 0; i < N; ++i)
				if (!(src = L::munch(src, end))) return src;
			return src;
		}
	};

	template <size_t N, typename L>
	constexpr Exactly_N_Times<L, N> exactly(const L& l)
	{ return Exactly_N_Times<L, N> { }; }

	template <typename L>
	constexpr Exactly_N_Times<L, 1> exactly(const L& l)
	{ return exactly<1>(l); }

	void bar() {
		exactly<5>(Epsilon { });
		exactly(Epsilon { });
	}

	template <const char* prefix>
	struct Exactly_This {
		static const char* munch(const char* src, const char* end)
		{
			const char* pos = prefix;
			while (*pos && src < end && *src == *pos) ++src, ++pos;
			return *pos ? src : nullptr;
		}
	};

	template <const char* prefix>
	constexpr Exactly_This<prefix> exactly()
	{ return Exactly_This<prefix> { }; }

	void f() {
		constexpr auto x = 5 + 5;
		constexpr auto x = 6 + 6;
	}

	expr() {
		// term plus or minus expression
		terms = { term() };
		while (lex("+" | "-")) {
			term << term();
		}
		return terms;
	}


}
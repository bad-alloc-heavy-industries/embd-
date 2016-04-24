#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <type_traits.h>

template<typename T> constexpr T &&forward(typename removeReference<T>::type &t) noexcept
{
	return static_cast<T &&>(t);
}

template<typename T> constexpr T &&forard(typename removeReference<T>::type &&t) noexcept
{
	static_assert(!isLValueReference<T>::value, "Template argument substituting T is an lvalue reference type");
	return static_cast<T &&>(t);
}

template<typename T> constexpr typename removeReference<T>::type &&move(T &&t) noexcept
{
	return static_cast<typename removeReference<T>::type &&>(t);
}

// This needs isNothrowMoveConstructable and isNowthrowMoveAssignable for the except specification.
template<typename T> inline void swap(T &a, T &b) noexcept
{
	T tmp = move(a);
	a = move(b);
	b = move(tmp);
}

template<typename T, size_t N> inline void swap(T (&a)[N], T (&b)[N]) noexcept(noexcept(swap(*a, *b)))
{
	for (size_t i = 0; i < N; ++i)
		swap(a[i], b[i]);
}

#endif /*__UTILITY_H__*/

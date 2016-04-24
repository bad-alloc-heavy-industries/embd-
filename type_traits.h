#ifndef __TYPE_TRAITS_H__
#define __TYPE_TRAITS_H__

#include <stddef.h>
#include <stdint.h>

template<typename T, T v> struct integralConstant
{
	static constexpr T value = v;
	typedef T valueType;
	typedef integralConstant<T, v> type;
	constexpr operator valueType() { return value; }
};
template<typename T, T v> constexpr T integralConstant<T, v>::value;

typedef integralConstant<bool, true> trueType;
typedef integralConstant<bool, false> falseType;

template<bool, typename, typename> struct conditional;
template<typename...> struct __or;
template<typename...> struct __and;
template<typename T> struct __not : public integralConstant<bool, !T::value> { };

template<> struct __or<> : public falseType { };
template<typename B> struct __or<B> : public B { };
template<typename B1, typename B2> struct __or<B1, B2> :
	public conditional<B1::value, B1, B2>::type { };
template<typename B1, typename B2, typename B3, typename... Bn> struct __or<B1, B2, B3, Bn...> :
	public conditional<B1::value, B1, __or<B2, B3, Bn...>>::type { };

template<> struct __and<> : public falseType { };
template<typename B> struct __and<B> : public B { };
template<typename B1, typename B2> struct __and<B1, B2> :
	public conditional<B1::value, B2, B1>::type { };
template<typename B1, typename B2, typename B3, typename... Bn> struct __and<B1, B2, B3, Bn...> :
	public conditional<B1::value, __and<B2, B3, Bn...>, B1>::type { };

struct __sfinaeTypes
{
	typedef uint8_t __one;
	typedef struct { uint8_t __arr[2]; } __two;
};

template<typename> struct removeCV;

template<typename> struct __isVoid : public falseType { };
template<> struct __isVoid<void> : public trueType { };
template<typename T> struct isVoid : public integralConstant<bool,
	__isVoid<typename removeCV<T>::type>::value> { };

template<typename> struct __isBoolean : public falseType { };
template<> struct __isBoolean<bool> : public trueType { };
template<typename T> struct isBoolean : public integralConstant<bool,
	__isBoolean<typename removeCV<T>::type>::value> { };

template<typename> struct __isIntegral : public falseType { };
template<> struct __isIntegral<bool> : public trueType { };
template<> struct __isIntegral<char> : public trueType { };
template<> struct __isIntegral<signed char> : public trueType { };
template<> struct __isIntegral<unsigned char> : public trueType { };
//template<> struct __isIntegral<short> : public trueType { };
template<> struct __isIntegral<signed short> : public trueType { };
template<> struct __isIntegral<unsigned short> : public trueType { };
//template<> struct __isIntegral<int> : public trueType { };
template<> struct __isIntegral<signed int> : public trueType { };
template<> struct __isIntegral<unsigned int> : public trueType { };
//template<> struct __isIntegral<long> : public trueType { };
template<> struct __isIntegral<signed long> : public trueType { };
template<> struct __isIntegral<unsigned long> : public trueType { };
template<typename T> struct isIntegral : public integralConstant<bool,
	__isIntegral<typename removeCV<T>::type>::value> { };

template<typename> struct isArray : public falseType { };
template<typename T, size_t N> struct isArray<T[N]> : public trueType { };
template<typename T> struct isArray<T[]> : public trueType { };

template<typename> struct isLValueReference : public falseType { };
template<typename T> struct isLValueReference<T &> : public trueType { };
template<typename> struct isRValueReference : public falseType { };
template<typename T> struct isRValueReference<T &&> : public trueType { };

template<typename> struct isFunction;
template<typename> struct isFunction : public falseType { };
template<typename Fn, typename... ArgTypes> struct isFunction<Fn(ArgTypes...)> :
	public trueType { };
template<typename Fn, typename... ArgTypes> struct isFunction<Fn(ArgTypes......)> :
	public trueType { };
template<typename Fn, typename... ArgTypes> struct isFunction<Fn(ArgTypes...) const> :
	public trueType { };
template<typename Fn, typename... ArgTypes> struct isFunction<Fn(ArgTypes......) const> :
	public trueType { };
template<typename Fn, typename... ArgTypes> struct isFunction<Fn(ArgTypes...) volatile> :
	public trueType { };
template<typename Fn, typename... ArgTypes> struct isFunction<Fn(ArgTypes......) volatile> :
	public trueType { };
template<typename Fn, typename... ArgTypes> struct isFunction<Fn(ArgTypes...) const volatile> :
	public trueType { };
template<typename Fn, typename... ArgTypes> struct isFunction<Fn(ArgTypes......) const volatile> :
	public trueType { };

template<typename T> struct isReference : public __or<isLValueReference<T>,
	isRValueReference<T>>::type { };

template<typename> struct isConst : public falseType { };
template<typename T> struct isConst<T const> : public trueType { };
template<typename> struct isVolatile : public falseType { };
template<typename T> struct isVolatile<T volatile> : public trueType { };

template<typename> struct addRValueReference;
template<typename T> typename addRValueReference<T>::type declval() noexcept;

template<typename, typename> struct isSame;
template<typename, typename> struct isBaseOf;
template<typename> struct removeReference;

template<typename, typename> struct isSame : public falseType { };
template<typename T> struct isSame<T, T> : public trueType { };

template<typename Base, typename Derived> struct isBaseOf :
	public integralConstant<bool, __is_base_of(Base, Derived)> { };

template<typename From, typename To, bool = __or<isVoid<From>, isFunction<To>, isArray<To>>::value>
	struct __isConvertible { static constexpr bool value = isVoid<To>::value; };
template<typename From, typename To> class __isConvertible<From, To, false> : public __sfinaeTypes
{
	template<typename To1> static void __testAux(To1);
	template<typename From1, typename To1> static
		decltype(__testAux<To1>(declval<From1>()), __one()) __test(int);
	template<typename, typename> static __two __test(...);

public:
	static constexpr bool value = sizeof(__test<From, To>(0)) == sizeof(uint8_t);
};
template<typename From, typename To> struct isConvertible : public integralConstant<bool,
	__isConvertible<From, To>::value> { };

template<typename T> struct removeConst { typedef T type; };
template<typename T> struct removeConst<T const> { typedef T type; };

template<typename T> struct removeVolatile { typedef T type; };
template<typename T> struct removeVolatile<T volatile> { typedef T type; };

template<typename T> struct removeCV
{
	typedef typename removeConst<typename removeVolatile<T>::type>::type type;
};

template<typename T> struct addConst { typedef const T type; };
template<typename T> struct addVolatile { typedef volatile T type; };
template<typename T> struct addCV
{
	typedef typename addConst<typename addVolatile<T>::type>::type type;
};

template<typename T> struct removeReference { typedef T type; };
template<typename T> struct removeReference<T &> { typedef T type; };
template<typename T> struct removeReference<T &&> { typedef T type; };

template<typename Unqualified, bool IsConst, bool IsVol> struct __cvSelector;
template<typename Unqualified> struct __cvSelector<Unqualified, false, false> { typedef Unqualified type; };
template<typename Unqualified> struct __cvSelector<Unqualified, false, true> { typedef volatile Unqualified type; };
template<typename Unqualified> struct __cvSelector<Unqualified, true, false> { typedef const Unqualified type; };
template<typename Unqualified> struct __cvSelector<Unqualified, true, true> { typedef const volatile Unqualified type; };
template<typename Qualified, typename Unqualified, bool IsConst = isConst<Qualified>::value,
	bool IsVol = isVolatile<Qualified>::value> struct matchCV
{
	typedef typename __cvSelector<Unqualified, IsConst, IsVol>::type type;
};

template<typename T> struct __makeUnsigned { typedef T type; };
template<> struct __makeUnsigned<char> { typedef unsigned char type; };
template<> struct __makeUnsigned<signed char> { typedef unsigned char type; };
template<> struct __makeUnsigned<signed short> { typedef unsigned short type; };
template<> struct __makeUnsigned<signed int> { typedef unsigned int type; };
template<> struct __makeUnsigned<signed long> { typedef unsigned long type; };
template<> struct __makeUnsigned<signed long long> { typedef unsigned long long type; };
template<typename T, bool = isIntegral<T>::value> struct __makeUnsignedSelect;
template<typename T> struct __makeUnsignedSelect<T, true>
{
	typedef __makeUnsigned<typename removeCV<T>::type> __unsignedType;
	typedef typename __unsignedType::type __unsigned;
	typedef matchCV<T, __unsigned> __unsignedCV;

public:
	typedef typename __unsignedCV::type type;
};
template<typename T> struct __makeUnsignedSelect<T, false> { typedef T type; };
template<typename T> struct makeUnsigned { typedef typename __makeUnsignedSelect<T>::type type; };
template<> struct makeUnsigned<bool>;

template<typename T> struct removeExtent { typedef T type; };
template<typename T, size_t N> struct removeExtent<T [N]> { typedef T type; };
template<typename T> struct removeExtent<T []> { typedef T type; };

template<typename T> struct removeAllExtents { typedef T type; };
template<typename T, size_t N> struct removeAllExtents<T [N]>
	{ typedef typename removeAllExtents<T>::type type; };
template<typename T> struct removeAllExtents<T []>
	{ typedef typename removeAllExtents<T>::type type; };

template<typename T> struct addPointer
	{ typedef typename removeReference<T>::type *type; };

template<typename T, bool = isIntegral<T>::value> struct __isSigned : public falseType { };
template<typename T> struct __isSigned<T, true> : public
	integralConstant<bool, static_cast<bool>(T(-1) < T(0))> { };
template<typename T> struct isSigned : public integralConstant<bool, __isSigned<T>::value> { };
template<typename T> struct isUnsigned : public __not<isSigned<T>>::type { };

template<typename T, bool = __and<__not<isReference<T>>, __not<isVoid<T>>>::value>
	struct __addRValueReference { typedef T type; };
template<typename T> struct __addRValueReference<T, true> { typedef T &&type; };
template<typename T> struct addRValueReference : public __addRValueReference<T> { };

template<typename T, bool = isArray<T>::value, bool = isFunction<T>::value> struct __decay;
template<typename T> struct __decay<T, false, false> { typedef typename removeCV<T>::type type; };
template<typename T> struct __decay<T, true, false> { typedef typename removeExtent<T>::type *type; };
template<typename T> struct __decay<T, false, true> { typedef typename addPointer<T>::type type; };
template<typename T> class decay
{
	typedef typename removeReference<T>::type removeType;

public:
	typedef typename __decay<removeType>::type type;
};

template<typename T> class referenceWrapper;
template<typename T> struct __stripReference { typedef T type; };
template<typename T> struct __stripReference<referenceWrapper<T>> { typedef T &type; };
template<typename T> struct __stripReference<const referenceWrapper<T>> { typedef T &type; };
template<typename T> class __decayAndStrip
{
	typedef typename decay<T>::type decayType;

public:
	typedef typename __stripReference<decayType>::type type;
};


template<bool, typename T = void> struct enableIf { };
template<typename T> struct enableIf<true, T> { typedef T type; };

template<bool, typename ifTrue, typename ifFalse> struct conditional { typedef ifTrue type; };
template<typename ifTrue, typename ifFalse> struct conditional<false, ifTrue, ifFalse>
	{ typedef ifFalse type; };

#endif /*__TYPE_TRAITS_H__*/

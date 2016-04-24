#ifndef __STDOUT_H__
#define __STDOUT_H__

#include <type_traits.h>
#include <utility.h>
#include <array.h>
#include <functional.h>

struct outDev
{
protected:
	typedef const call<void()> init_t;
	typedef const call<void(const char c)> writeChar_t;
	typedef const call<void(const char *str)> writeString_t;

	struct functions
	{
	public:
		const init_t init;
		const writeChar_t writeChar;
		const writeString_t writeString;

		functions() = delete;
		constexpr functions(functions &&fns) noexcept : init(fns.init), writeChar(fns.writeChar), writeString(fns.writeString) { }
		constexpr functions(const functions &fns) noexcept : init(fns.init), writeChar(fns.writeChar), writeString(fns.writeString) { }
		constexpr functions(const init_t initFn, const writeChar_t writeCharFn, const writeString_t writeStringFn) noexcept :
			init(initFn), writeChar(writeCharFn), writeString(writeStringFn) { }
	};
	const functions *const vtable;
	void *const instance;

	constexpr outDev(const functions *const table, void *const inst) noexcept : vtable(table), instance(inst) { }

public:
	void init() noexcept { vtable->init(instance); }
	void write(const char c) noexcept { vtable->writeChar(instance, c); }
	void write(const char *str) noexcept { vtable->writeString(instance, str); }
};

struct printable_t { };

template<uint8_t pad = 0, uint8_t padChar = ' '> struct asHex : public printable_t
{
private:
	static constexpr uint8_t padding = pad;
	const uint8_t maxDigits;
	const uint8_t msbShift;
	const uint32_t number;

public:
	template<typename T> constexpr asHex(const T value) noexcept : maxDigits(sizeof(T) * 2), msbShift(4 * (maxDigits - 1)), number(value) { }

	[[gnu::noinline]]
	void operator ()(outDev &dev)
	{
		uint8_t i;
		uint32_t value(number);
		// If we've been asked to pad by more than the maximum possible length of the number
		if (maxDigits < pad)
		{
			// Put out the excess padding early to keep the logic simple.
			for (i = 0; i < (pad - maxDigits); ++i)
				dev.write(padChar);
		}

		// For up to the maximum number of digits, pad as needed
		for (i = maxDigits; i > 1; i--)
		{
			const uint8_t nibble = uint8_t((value >> msbShift) & 0x0F);
			if (nibble == 0)
				value <<= 4;
			if (i > pad && nibble == 0)
				continue;
			else if (i <= pad && nibble == 0)
				dev.write(padChar);
			else
				break;
			// If 0 and pad == 0, we don't output anything here.
		}

		for (; i > 0; --i)
		{
			const uint8_t nibble = uint8_t((value >> msbShift) & 0x0F);
			const uint8_t ch = nibble + '0';
			if (ch > '9')
				dev.write(ch + 7);
			else
				dev.write(ch);
			value <<= 4;
		}
	}
};

template<typename N> struct asInt : public printable_t
{
private:
	typedef typename makeUnsigned<N>::type UInt;
	const N number;

	[[gnu::noinline]] UInt print(const UInt number, outDev &dev) noexcept
	{
		if (number < 10)
			dev.write(number + '0');
		else
		{
			const UInt num = number - (print(number / 10, dev) * 10);
			dev.write(num + '0');
		}
		return number;
	}

	template<typename T> typename enableIf<isSame<T, N>::value && isIntegral<T>::value && !isBoolean<T>::value && isUnsigned<T>::value>::type
		format(outDev &dev)
	{
		print(number, dev);
	}

	template<typename T> [[gnu::noinline]] typename enableIf<isSame<T, N>::value && isIntegral<T>::value && !isBoolean<T>::value && isSigned<T>::value>::type
		format(outDev &dev)
	{
		if (number < 0)
		{
			dev.write('-');
			print((typename makeUnsigned<N>::type)-number, dev);
		}
		else
			print((typename makeUnsigned<N>::type)number, dev);
	}

public:
	constexpr asInt(const N value) noexcept : number(value) { }
	void operator ()(outDev &dev) noexcept { format<N>(dev); }
};

template<typename> struct isChar : falseType { };
template<> struct isChar<char> : trueType { };
template<typename T> struct isScalar : public integralConstant<bool,
	isIntegral<T>::value && !isBoolean<T>::value && !isChar<T>::value> { };

struct stdout_t
{
private:
	outDev &dev;

	void print(const char *value) noexcept { dev.write(value); }
	/*void print(function<void(outDev &)> callable) noexcept { callable(dev); }*/
	void print(const char value) noexcept { dev.write(value); }

	template<typename T> typename enableIf<isBaseOf<printable_t, T>::value>::type
		print(T &printable) noexcept { printable(dev); }

	template<typename T> typename enableIf<isScalar<T>::value>::type
		print(T &num) noexcept { write(asInt<T>(num)); }

	template<typename T> void print(T *ptr) noexcept
	{
		write("0x", asHex<8, '0'>((const long)ptr));
	}

	void print(const bool value) noexcept
	{
		if (value)
			dev.write("true");
		else
			dev.write("false");
	}

	template<typename T, size_t N> void print(array<T, N> &arr) noexcept
	{
		for (auto &elem : arr)
			write(elem);
	}

public:
	constexpr stdout_t(outDev &device) noexcept : dev(device) { }
	void init() noexcept { dev.init(); }
	stdout_t &write() noexcept { return *this; }

	template<typename T, typename... U> stdout_t &write(T value, U... values) noexcept
	{
		print(value);
		return write(values...);
	}

	stdout_t() = delete;
	stdout_t(const stdout_t &) = delete;
	stdout_t(stdout_t &&) = delete;
	stdout_t &operator =(const stdout_t &) = delete;
	stdout_t &operator =(stdout_t &&) = delete;
};

extern stdout_t stdout;

#endif /*__STDOUT_H__*/

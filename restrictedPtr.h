#ifndef __restrictedPtr_H__
#define __restrictedPtr_H__

template<typename T, uint32_t mask = 0xFFFFFFF0>
class restrictedPtr_t
{
private:
	typedef T *ptr_t;
	ptr_t ptr;
	static_assert(mask > 0, "restrictedPtr_t: mask cannot be 0");
	constexpr static const uint32_t restrictMask = mask;

public:
	constexpr restrictedPtr_t() noexcept : ptr(nullptr) { }
	constexpr restrictedPtr_t(nullptr_t) noexcept : ptr(nullptr) { }
	operator ptr_t() const noexcept { return ptr & restrictMask; }
	T &operator *() noexcept { return *ptr_t(*this); }

	restrictedPtr_t(ptr_t p) noexcept : restrictedPtr_t()
	{
		*this = p;
	}

	void operator =(ptr_t p) volatile noexcept
	{
		long value = reinterpret_cast<long>(p) & restrictMask;
		ptr = reinterpret_cast<ptr_t>(value);
	}

	bool operator ==(const restrictedPtr_t<T> &p) volatile noexcept
	{
		return ptr_t(p) == ptr_t(*this);
	}

	bool operator ==(ptr_t p) volatile noexcept
	{
		return ptr_t(*this) == p;
	}

	explicit operator bool() volatile noexcept { return ptr != nullptr; }
	explicit operator uint32_t() volatile noexcept
	{
		static_assert(sizeof(uint32_t) == sizeof(long), "operator uint32_t() on restrictedPtr_t is invalid");
		return uint32_t(reinterpret_cast<long>(ptr));
	}
};

#endif /*__restrictedPtr_H__*/

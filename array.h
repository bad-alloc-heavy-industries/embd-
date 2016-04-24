#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stddef.h>
#include <utility.h>

template<class T> class initializer_list
{
public:
	typedef T value_type;
	typedef const T &reference;
	typedef const T &constReference;
	typedef const T *iterator;
	typedef const T *constIterator;

private:
	iterator array;
	size_t len;

	constexpr initializer_list(constIterator a, size_t l) : array(a), len(l) { }

public:
	constexpr initializer_list() noexcept : array(0), len(0) { }
	constexpr size_t size() const noexcept { return len; }
	constexpr constIterator begin() const noexcept { return array; }
	constexpr constIterator end() const noexcept { return begin() + size(); }
};

template<class T> constexpr const T *begin(initializer_list<T> ils) noexcept
{
	return ils.begin();
}

template<class T> constexpr const T *end(initializer_list<T> ils) noexcept
{
	return ils.end();
}

template<typename T, size_t N> struct array
{
public:
	typedef T *iterator;
	typedef const T *constIterator;

private:
	T arr[N];

public:
	constexpr array() noexcept { }
	constexpr array(initializer_list<T> value) noexcept : arr(value) { }
	template<typename... U> constexpr array(U... values) noexcept : arr{T(values)...} { }

	array(const array<T, N> &a) noexcept
	{
		for (size_t i = 0; i < N; ++i)
			arr[i] = a.arr[i];
	}

	array(array<T, N> &&a) noexcept
	{
		swap(a);
	}

	array &operator =(const array<T, N> &a) noexcept
	{
		for (size_t i = 0; i < N; ++i)
			arr[i] = a.arr[i];
		return *this;
	}

	array &operator =(array<T, N> &&a) noexcept
	{
		swap(a);
		return *this;
	}

	void swap(array<T, N> &a) noexcept
	{
		::swap(arr, a.arr);
	}

	T &operator [](const size_t index) noexcept
	{
		if (index >= N)
			return arr[0]; /* On invalid index, to avoid exceptions, just return the 1st element */
		return arr[index];
	}

	const T operator[](const size_t index) const noexcept
	{
		if (index >= N)
			return 0;
		return arr[index];
	}

	T *data() noexcept
	{
		return arr;
	}

	constexpr const T *data() const noexcept
	{
		return arr;
	}

	void clear() noexcept
	{
		for (size_t i = 0; i < N; i++)
			arr[i] = 0;
	}

	constexpr size_t size() const noexcept { return N; }
	iterator begin() noexcept { return arr; }
	constexpr constIterator begin() const noexcept { return arr; }
	iterator end() noexcept { return end() + size(); }
	constexpr constIterator end() const noexcept { return begin() + size(); }
};

template<typename T> struct iterate
{
public:
	typedef T *iterator;
	typedef const T *constIterator;

private:
	T *array;
	const size_t len;

public:
	constexpr iterate(void *addr, size_t size) noexcept : array(reinterpret_cast<T *>(addr)), len(size) { }
	constexpr size_t size() const noexcept { return len; }
	constexpr constIterator begin() const noexcept { return array; }
	iterator begin() noexcept { return array; }
	constexpr constIterator end() const noexcept { return begin() + size(); }
	iterator end() noexcept { return begin() + size(); }
};

struct arrayAt
{
private:
	void *array;
	const uint32_t len;

public:
	constexpr arrayAt(const long addr, const uint32_t size) : array(reinterpret_cast<void *>(addr)), len(size) { }
	constexpr size_t size() const noexcept { return len; }
	template<typename T> iterate<T> as() noexcept { return iterate<T>(array, len / sizeof(T)); }
	template<typename T> constexpr const iterate<T> as() const noexcept { return iterate<T>(array, len / sizeof(T)); }

	arrayAt() = delete;
	arrayAt(const arrayAt &) = delete;
	arrayAt(arrayAt &&) = delete;
	arrayAt &operator =(const arrayAt &) = delete;
	arrayAt &operator =(arrayAt &&) = delete;
};

#endif /*__ARRAY_H__*/

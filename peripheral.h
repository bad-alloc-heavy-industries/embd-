#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

template<typename T> struct peripheral
{
protected:
	volatile T *const ptr;

public:
	constexpr peripheral(const long addr) noexcept : ptr((volatile T *const)addr) { }
	constexpr operator volatile T *() const noexcept { return ptr; }
	constexpr volatile T *operator ->() const noexcept { return ptr; }
	constexpr volatile T *addr() const noexcept { return ptr; }

	peripheral() = delete;
	peripheral(const peripheral<T> &) = delete;
	peripheral(peripheral<T> &&) = delete;

	peripheral<T> &operator =(peripheral<T> &) = delete;
	peripheral<T> &operator =(peripheral<T> &&) = delete;
};

template<typename T, long address> struct constPeripheral
{
protected:
	constexpr static volatile T *const ptr = (volatile T *const)address;

public:
	constexpr constPeripheral() noexcept { }
	constexpr operator volatile T *() const noexcept { return ptr; }
	constexpr volatile T *operator ->() const noexcept { return ptr; }
	constexpr volatile T *addr() const noexcept { return ptr; }

	constPeripheral(const constPeripheral<T, address> &) = delete;
	constPeripheral(constPeripheral<T, address> &&) = delete;

	constPeripheral<T, address> &operator =(constPeripheral<T, address> &) = delete;
	constPeripheral<T, address> &operator =(constPeripheral<T, address> &&) = delete;
};

#endif /*__PERIPHERAL_H__*/

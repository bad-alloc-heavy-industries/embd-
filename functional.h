#ifndef __FUNCTIONAL_H__
#define __FUNCTIONAL_H__

#include <type_traits.h>
#include <utility.h>

template<typename Signature> class function;
template<typename Func, typename... Args> class function<Func(Args...)>
{
private:
	struct objectType
	{
	private:
		void *const object;

	public:
		constexpr objectType() noexcept : object(nullptr) { }
		constexpr objectType(void *const obj) noexcept : object(obj) { }
		constexpr objectType(const void *const obj) noexcept : object(const_cast<void *>(obj)) { }
		template<class Class> constexpr objectType(Class &obj) noexcept : object(&obj) { }
		template<class Class> constexpr objectType(const Class &obj) noexcept : object(const_cast<Class *>(&obj)) { }
		constexpr operator void *() const noexcept { return object; }
	};

	typedef Func (*functor_t)(void *const, Args &&...);
	typedef void (*deleteType)(void *const);

	void *const obj;
	const deleteType deletePtr;
	const objectType objectPtr;
	const functor_t functor;

	/*template<Func (*functionPtr)(Args...)> static Func functorStub(const void *const, Args &&... args)
	{
		return functionPtr(forward<Args>(args)...);
	}*/
	static Func functorStub(void *const objectPtr, Args &&... args)
	{
		return (static_cast<Func (*)(Args...)>(objectPtr))(forward<Args>(args)...);
	}

	template<class Class, Func (Class::* memberPtr)(Args...)> static Func functorStub(void *const objectPtr, Args &&... args)
	{
		return (static_cast<Class *>(objectPtr)->* memberPtr)(forward<Args>(args)...);
	}

	template<class Class, Func (Class::* memberPtr)(Args...) const> static Func functorStub(void *const objectPtr, Args &&... args)
	{
		return (static_cast<const Class *>(objectPtr)->* memberPtr)(forward<Args>(args)...);
	}

	template<typename T> static Func functorStub(void *const objectPtr, Args &&... args)
	{
		return (*static_cast<T *>(objectPtr))(forward<Args>(args)...);
	}

	template<typename T> static void del(void *const objectPtr)
	{
		static_cast<T *>(objectPtr)->~T();
		operator delete(objectPtr);
	}

public:
	typedef Func resultType;

	constexpr function() noexcept : obj(nullptr), deletePtr(nullptr), objectPtr(nullptr), functor(nullptr) { }
	function(const nullptr_t) noexcept : obj(nullptr), deletePtr(nullptr), objectPtr(nullptr), functor(nullptr) { }
	//function(const function &fn) noexcept : objectPtr(fn.objectPtr), functor(fn.functor) { }
	//function(function &&x) : function() { x.swap(*this); }
	~function()
	{
		if (deletePtr)
			deletePtr(obj);
	}

	/*template<class Class, typename = typename enableIf<isClass<Class>::value>::type>
	explicit function(Class const *const obj) noexcept :
		obj(nullptr), deletePtr(nullptr), objectPtr(obj), functor(nullptr) { }
	template<class Class, typename = typename enableIf<isClass<Class>::value>::type>
	explicit function(Class const &obj) noexcept :
		obj(nullptr), deletePtr(nullptr), objectPtr(obj), functor(nullptr) { }*/

	//function(Func (*fn)(Args...)) : objectPtr(nullptr), functor(functorStub<fn>) { }
	//function(Func (*fn)(Args...)) : obj(nullptr), deletePtr(nullptr), objectPtr(fn), functor(functorStub) { }
	template<class Class> function(Class *const object, Func (Class::* const memberPtr)(Args...)) :
		obj(nullptr), deletePtr(nullptr), objectPtr(object), functor(functorStub<Class, memberPtr>) { }
	template<class Class> function(const Class *const object, Func (Class::* const memberPtr)(Args...) const) :
		obj(nullptr), deletePtr(nullptr), objectPtr(object), functor(functorStub<Class, memberPtr>) { }
	template<class Class> function(Class &object, Func (Class::* const memberPtr)(Args...)) :
		obj(nullptr), deletePtr(nullptr), objectPtr(object), functor(functorStub<Class, memberPtr>) { }
	template<class Class> function(const Class &object, Func (Class::* const memberPtr)(Args...) const) :
		obj(nullptr), deletePtr(nullptr), objectPtr(object), functor(functorStub<Class, memberPtr>) { }
	template<typename T, typename functorType = typename decay<T>::type, typename = typename enableIf<!isSame<function, functorType>::value>::type>
		function(T &&fn) : obj(operator new(sizeof(functorType))), deletePtr(del<functorType>), objectPtr(obj), functor(functorStub<functorType>)
	{
		new (objectPtr) functorType(forward<T>(fn));
	}

	explicit constexpr operator bool() const noexcept { return functor; }

	Func operator ()(Args... args) const
	{
		return functor(objectPtr, forward<Args>(args)...);
	}
};

template<typename Signature> struct call;
template<typename Func, typename... Args> struct call<Func(Args...)>
{
private:
	typedef Func (*const functor_t)(void *const, Args...args);
	const functor_t functor;

	template<class Class, Func (Class::* const ptr)(Args...)>
	constexpr static Func stub(void *const objectPtr, Args... args) noexcept
	{
		return (static_cast<Class *const>(objectPtr)->* ptr)(forward<Args>(args)...);
	}

	constexpr call(functor_t call) noexcept : functor(call) { }

public:
	call() = delete;
	constexpr call(const call &c) noexcept : functor(c.functor) { }
	constexpr call(call &&c) noexcept : functor(c.functor) { }

	template<class Class, Func (Class::* const ptr)(Args...)>
	constexpr static call make() noexcept
	{
		return call(stub<Class, ptr>);
	}

	Func operator()(void *const object, Args... args) const noexcept
	{
		return functor(object, forward<Args>(args)...);
	}
};

#endif /*__FUNCTIONAL_H__*/

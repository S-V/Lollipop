/*
=============================================================================
	File:	Delegate.h
	Desc:	
=============================================================================
*/

#ifndef __MX_TEMPLATES_DELEGATE_H__
#define __MX_TEMPLATES_DELEGATE_H__

// Based on http://www.codeproject.com/KB/cpp/CPPCallback.aspx
// See also http://molecularmusings.wordpress.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/

template< typename FuncSignature >
class TCallback;

struct NullCallback {};

// 0 parameter version

// returns 'R' and accepts 'void'
template< typename R >
class TCallback< R () >
{
	// generic function pointer type
	typedef R (*FuncType) (const void* pObject);

	FuncType 		m_func;
	const void *	m_obj;

	// actual binding happens here;
	// this ctor is called by friend classes.
	//
	inline TCallback( FuncType f, const void* o )
		: m_func(f), m_obj(o)
	{}

public:
	enum { Arity = 0 };
	typedef R ReturnType;

	inline TCallback()
		: m_func(nil), m_obj(nil)
	{}
	inline TCallback( NullCallback )
		: m_func(nil), m_obj(nil)
	{}
	inline TCallback( const TCallback& rhs )
		: m_func(rhs.m_func), m_obj(rhs.m_obj)
	{}
	inline ~TCallback()
	{}

	inline TCallback& operator = ( NullCallback )
	{
		m_obj = nil;
		m_func = nil;
		return *this;
	}
	inline TCallback& operator = ( const TCallback& rhs )
	{
		m_obj = rhs.m_obj;
		m_func = rhs.m_func;
		return *this;
	}

	// for storing in containers
	inline bool operator == ( const TCallback& rhs ) const
	{
		return m_obj == rhs.m_obj
			&& m_func == rhs.m_func
			;
	}
	inline bool operator != ( const TCallback& rhs ) const
	{
		return !this->operator==(rhs);
	}

	inline R operator () (void) const
	{
		AssertPtr(m_obj);
		AssertPtr(m_func);
		// calls the templated wrapper function
		return (*m_func)(m_obj);
	}

	template< class KLASS >
	inline const KLASS* ToClassPtr() const
	{
		return static_cast< const KLASS* >( m_obj );
	}

private:
	typedef const void* TCallback::*SafeBoolType;
public:
	inline operator SafeBoolType() const
	{
		return m_func != nil ? &TCallback::m_obj : nil;
	}
	inline bool operator!() const
	{
		return m_func == nil;
	}

private:
	template<typename FR>
	friend class FreeCallbackFactory0;

	template<typename FR, class FT>
	friend class MemberCallbackFactory0;

	template<typename FR, class FT>
	friend class ConstMemberCallbackFactory0;
};

template<typename R>
void operator==(const TCallback<R ()>&,
				const TCallback<R ()>&);
template<typename R>
void operator!=(const TCallback<R ()>&,
				const TCallback<R ()>&);

//---------------------------------------------------------------------------
// N=0, Non-member function

template<typename R>
class FreeCallbackFactory0
{
private:
	template<R (*Func)()>
	static R Wrapper(const void*)
	{
		return (*Func)();
	}

public:
	template<R (*Func)()>
	inline static TCallback<R ()> MakeCallback()
	{
		return TCallback<R ()>
			(&FreeCallbackFactory0::Wrapper<Func>, nil);
	}
};

template<typename R>
inline FreeCallbackFactory0<R>
GetCallbackFactory(R (*)())
{
	return FreeCallbackFactory0<R>();
}

//---------------------------------------------------------------------------
// N=0, Non-Const member function

template<typename R, class T>
class MemberCallbackFactory0
{
private:
	template< R (T::*Func)() >
	inline static R Wrapper(const void* o)
	{
		T* obj = const_cast<T*>(static_cast<const T*>(o));
		return (obj->*Func)();
	}

public:
	template< R (T::*Func)() >
	inline static TCallback<R ()> MakeCallback( T& o )
	{
		return TCallback< R () >
		(
			&MemberCallbackFactory0::Wrapper<Func>,
			static_cast<const void*>( &o )
		);
	}
};

template< typename R, class T >
inline MemberCallbackFactory0< R, T >
GetCallbackFactory( R (T::*)() )
{
	return MemberCallbackFactory0< R, T >();
}

//---------------------------------------------------------------------------
// N=0, Const member function

template<typename R, class T>
class ConstMemberCallbackFactory0
{
private:
	template<R (T::*Func)() const>
	static R Wrapper(const void* o)
	{
		const T* obj = static_cast<const T*>(o);
		return (obj->*Func)();
	}

public:
	template< R (T::*Func)() const >
	inline static TCallback<R ()> MakeCallback( const T& o )
	{
		return TCallback<R ()>
		(
			&ConstMemberCallbackFactory0::Wrapper<Func>,
			static_cast<const void*>(&o)
		);
	}
};

template< typename R, class T >
inline ConstMemberCallbackFactory0< R, T >
GetCallbackFactory( R (T::*)() const )
{
	return ConstMemberCallbackFactory0< R, T >();
}

//---------------------------------------------------------------------------

// 1 parameter version

template<typename R, typename P1>
class TCallback<R (P1)>
{
public:
	static const int Arity = 1;
	typedef R ReturnType;
	typedef P1 Param1Type;

	TCallback()                    : func(0), obj(0) {}
	TCallback(NullCallback)        : func(0), obj(0) {}
	TCallback(const TCallback& rhs) : func(rhs.func), obj(rhs.obj) {}
	~TCallback() {} 

	TCallback& operator=(NullCallback)
	{ obj = 0; func = 0; return *this; }
	TCallback& operator=(const TCallback& rhs)
	{ obj = rhs.obj; func = rhs.func; return *this; }

	inline R operator()(P1 a1) const
	{
		return (*func)(obj, a1);
	}
	// for storing in containers
	inline bool operator == ( const TCallback& rhs ) const
	{
		return obj == rhs.obj
			&& func == rhs.func
			;
	}
	inline bool operator != ( const TCallback& rhs ) const
	{
		return !this->operator==(rhs);
	}
	template< class KLASS >
	inline const KLASS* ToClassPtr() const
	{
		return static_cast< const KLASS* >( obj );
	}
private:
	typedef const void* TCallback::*SafeBoolType;
public:
	inline operator SafeBoolType() const
	{ return func != 0 ? &TCallback::obj : 0; }
	inline bool operator!() const
	{ return func == 0; }

private:
	typedef R (*FuncType)(const void*, P1);
	TCallback(FuncType f, const void* o) : func(f), obj(o) {}

private:
	FuncType func;
	const void* obj;

	template<typename FR, typename FP1>
	friend class FreeCallbackFactory1;
	template<typename FR, class FT, typename FP1>
	friend class MemberCallbackFactory1;
	template<typename FR, class FT, typename FP1>
	friend class ConstMemberCallbackFactory1;
};

template<typename R, typename P1>
void operator==(const TCallback<R (P1)>&,
				const TCallback<R (P1)>&);
template<typename R, typename P1>
void operator!=(const TCallback<R (P1)>&,
				const TCallback<R (P1)>&);

template<typename R, typename P1>
class FreeCallbackFactory1
{
private:
	template<R (*Func)(P1)>
	static R Wrapper(const void*, P1 a1)
	{
		return (*Func)(a1);
	}

public:
	template<R (*Func)(P1)>
	inline static TCallback<R (P1)> MakeCallback()
	{
		return TCallback<R (P1)>
			(&FreeCallbackFactory1::Wrapper<Func>, nil);
	}
};

template<typename R, typename P1>
inline FreeCallbackFactory1<R, P1>
GetCallbackFactory(R (*)(P1))
{
	return FreeCallbackFactory1<R, P1>();
}

template<typename R, class T, typename P1>
class MemberCallbackFactory1
{
private:
	template<R (T::*Func)(P1)>
	static R Wrapper(const void* o, P1 a1)
	{
		T* obj = const_cast<T*>(static_cast<const T*>(o));
		return (obj->*Func)(a1);
	}

public:
	template<R (T::*Func)(P1)>
	inline static TCallback<R (P1)> MakeCallback(T& o)
	{
		return TCallback<R (P1)>
			(&MemberCallbackFactory1::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1>
inline MemberCallbackFactory1<R, T, P1>
GetCallbackFactory(R (T::*)(P1))
{
	return MemberCallbackFactory1<R, T, P1>();
}

template<typename R, class T, typename P1>
class ConstMemberCallbackFactory1
{
private:
	template<R (T::*Func)(P1) const>
	static R Wrapper(const void* o, P1 a1)
	{
		const T* obj = static_cast<const T*>(o);
		return (obj->*Func)(a1);
	}

public:
	template<R (T::*Func)(P1) const>
	inline static TCallback<R (P1)> MakeCallback(const T& o)
	{
		return TCallback<R (P1)>
			(&ConstMemberCallbackFactory1::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1>
inline ConstMemberCallbackFactory1<R, T, P1>
GetCallbackFactory(R (T::*)(P1) const)
{
	return ConstMemberCallbackFactory1<R, T, P1>();
}

// 2 parameter version

template<typename R, typename P1, typename P2>
class TCallback<R (P1, P2)>
{
public:
	static const int Arity = 2;
	typedef R ReturnType;
	typedef P1 Param1Type;
	typedef P2 Param2Type;

	TCallback()                    : func(0), obj(0) {}
	TCallback(NullCallback)        : func(0), obj(0) {}
	TCallback(const TCallback& rhs) : func(rhs.func), obj(rhs.obj) {}
	~TCallback() {} 

	TCallback& operator=(NullCallback)
	{ obj = 0; func = 0; return *this; }
	TCallback& operator=(const TCallback& rhs)
	{ obj = rhs.obj; func = rhs.func; return *this; }

	inline R operator()(P1 a1, P2 a2) const
	{
		return (*func)(obj, a1, a2);
	}

	// for storing in containers
	inline bool operator == ( const TCallback& rhs ) const
	{
		return obj == rhs.obj
			&& func == rhs.func
			;
	}
	inline bool operator != ( const TCallback& rhs ) const
	{
		return !this->operator==(rhs);
	}
	template< class KLASS >
	inline const KLASS* ToClassPtr() const
	{
		return static_cast< const KLASS* >( obj );
	}
private:
	typedef const void* TCallback::*SafeBoolType;
public:
	inline operator SafeBoolType() const
	{ return func != 0 ? &TCallback::obj : 0; }
	inline bool operator!() const
	{ return func == 0; }

private:
	typedef R (*FuncType)(const void*, P1, P2);
	TCallback(FuncType f, const void* o) : func(f), obj(o) {}

private:
	FuncType func;
	const void* obj;

	template<typename FR, typename FP1, typename FP2>
	friend class FreeCallbackFactory2;
	template<typename FR, class FT, typename FP1, typename FP2>
	friend class MemberCallbackFactory2;
	template<typename FR, class FT, typename FP1, typename FP2>
	friend class ConstMemberCallbackFactory2;
};

template<typename R, typename P1, typename P2>
void operator==(const TCallback<R (P1, P2)>&,
				const TCallback<R (P1, P2)>&);
template<typename R, typename P1, typename P2>
void operator!=(const TCallback<R (P1, P2)>&,
				const TCallback<R (P1, P2)>&);

template<typename R, typename P1, typename P2>
class FreeCallbackFactory2
{
private:
	template<R (*Func)(P1, P2)>
	static R Wrapper(const void*, P1 a1, P2 a2)
	{
		return (*Func)(a1, a2);
	}

public:
	template<R (*Func)(P1, P2)>
	inline static TCallback<R (P1, P2)> MakeCallback()
	{
		return TCallback<R (P1, P2)>
			(&FreeCallbackFactory2::Wrapper<Func>, nil);
	}
};

template<typename R, typename P1, typename P2>
inline FreeCallbackFactory2<R, P1, P2>
GetCallbackFactory(R (*)(P1, P2))
{
	return FreeCallbackFactory2<R, P1, P2>();
}

template<typename R, class T, typename P1, typename P2>
class MemberCallbackFactory2
{
private:
	template<R (T::*Func)(P1, P2)>
	static R Wrapper(const void* o, P1 a1, P2 a2)
	{
		T* obj = const_cast<T*>(static_cast<const T*>(o));
		return (obj->*Func)(a1, a2);
	}

public:
	template<R (T::*Func)(P1, P2)>
	inline static TCallback<R (P1, P2)> MakeCallback(T& o)
	{
		return TCallback<R (P1, P2)>
			(&MemberCallbackFactory2::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1, typename P2>
inline MemberCallbackFactory2<R, T, P1, P2>
GetCallbackFactory(R (T::*)(P1, P2))
{
	return MemberCallbackFactory2<R, T, P1, P2>();
}

template<typename R, class T, typename P1, typename P2>
class ConstMemberCallbackFactory2
{
private:
	template<R (T::*Func)(P1, P2) const>
	static R Wrapper(const void* o, P1 a1, P2 a2)
	{
		const T* obj = static_cast<const T*>(o);
		return (obj->*Func)(a1, a2);
	}

public:
	template<R (T::*Func)(P1, P2) const>
	inline static TCallback<R (P1, P2)> MakeCallback(const T& o)
	{
		return TCallback<R (P1, P2)>
			(&ConstMemberCallbackFactory2::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1, typename P2>
inline ConstMemberCallbackFactory2<R, T, P1, P2>
GetCallbackFactory(R (T::*)(P1, P2) const)
{
	return ConstMemberCallbackFactory2<R, T, P1, P2>();
}

// 3 parameter version

template<typename R, typename P1, typename P2, typename P3>
class TCallback<R (P1, P2, P3)>
{
public:
	static const int Arity = 3;
	typedef R ReturnType;
	typedef P1 Param1Type;
	typedef P2 Param2Type;
	typedef P3 Param3Type;

	TCallback()                    : func(0), obj(0) {}
	TCallback(NullCallback)        : func(0), obj(0) {}
	TCallback(const TCallback& rhs) : func(rhs.func), obj(rhs.obj) {}
	~TCallback() {} 

	TCallback& operator=(NullCallback)
	{ obj = 0; func = 0; return *this; }
	TCallback& operator=(const TCallback& rhs)
	{ obj = rhs.obj; func = rhs.func; return *this; }

	inline R operator()(P1 a1, P2 a2, P3 a3) const
	{
		return (*func)(obj, a1, a2, a3);
	}
	// for storing in containers
	inline bool operator == ( const TCallback& rhs ) const
	{
		return obj == rhs.obj
			&& func == rhs.func
			;
	}
	inline bool operator != ( const TCallback& rhs ) const
	{
		return !this->operator==(rhs);
	}
	template< class KLASS >
	inline const KLASS* ToClassPtr() const
	{
		return static_cast< const KLASS* >( obj );
	}
private:
	typedef const void* TCallback::*SafeBoolType;
public:
	inline operator SafeBoolType() const
	{ return func != 0 ? &TCallback::obj : 0; }
	inline bool operator!() const
	{ return func == 0; }

private:
	typedef R (*FuncType)(const void*, P1, P2, P3);
	TCallback(FuncType f, const void* o) : func(f), obj(o) {}

private:
	FuncType func;
	const void* obj;

	template<typename FR, typename FP1, typename FP2, typename FP3>
	friend class FreeCallbackFactory3;
	template<typename FR, class FT, typename FP1, typename FP2, typename FP3>
	friend class MemberCallbackFactory3;
	template<typename FR, class FT, typename FP1, typename FP2, typename FP3>
	friend class ConstMemberCallbackFactory3;
};

template<typename R, typename P1, typename P2, typename P3>
void operator==(const TCallback<R (P1, P2, P3)>&,
				const TCallback<R (P1, P2, P3)>&);
template<typename R, typename P1, typename P2, typename P3>
void operator!=(const TCallback<R (P1, P2, P3)>&,
				const TCallback<R (P1, P2, P3)>&);

template<typename R, typename P1, typename P2, typename P3>
class FreeCallbackFactory3
{
private:
	template<R (*Func)(P1, P2, P3)>
	static R Wrapper(const void*, P1 a1, P2 a2, P3 a3)
	{
		return (*Func)(a1, a2, a3);
	}

public:
	template<R (*Func)(P1, P2, P3)>
	inline static TCallback<R (P1, P2, P3)> MakeCallback()
	{
		return TCallback<R (P1, P2, P3)>
			(&FreeCallbackFactory3::Wrapper<Func>, nil);
	}
};

template<typename R, typename P1, typename P2, typename P3>
inline FreeCallbackFactory3<R, P1, P2, P3>
GetCallbackFactory(R (*)(P1, P2, P3))
{
	return FreeCallbackFactory3<R, P1, P2, P3>();
}

template<typename R, class T, typename P1, typename P2, typename P3>
class MemberCallbackFactory3
{
private:
	template<R (T::*Func)(P1, P2, P3)>
	static R Wrapper(const void* o, P1 a1, P2 a2, P3 a3)
	{
		T* obj = const_cast<T*>(static_cast<const T*>(o));
		return (obj->*Func)(a1, a2, a3);
	}

public:
	template<R (T::*Func)(P1, P2, P3)>
	inline static TCallback<R (P1, P2, P3)> MakeCallback(T& o)
	{
		return TCallback<R (P1, P2, P3)>
			(&MemberCallbackFactory3::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1, typename P2, typename P3>
inline MemberCallbackFactory3<R, T, P1, P2, P3>
GetCallbackFactory(R (T::*)(P1, P2, P3))
{
	return MemberCallbackFactory3<R, T, P1, P2, P3>();
}

template<typename R, class T, typename P1, typename P2, typename P3>
class ConstMemberCallbackFactory3
{
private:
	template<R (T::*Func)(P1, P2, P3) const>
	static R Wrapper(const void* o, P1 a1, P2 a2, P3 a3)
	{
		const T* obj = static_cast<const T*>(o);
		return (obj->*Func)(a1, a2, a3);
	}

public:
	template<R (T::*Func)(P1, P2, P3) const>
	inline static TCallback<R (P1, P2, P3)> MakeCallback(const T& o)
	{
		return TCallback<R (P1, P2, P3)>
			(&ConstMemberCallbackFactory3::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1, typename P2, typename P3>
inline ConstMemberCallbackFactory3<R, T, P1, P2, P3>
GetCallbackFactory(R (T::*)(P1, P2, P3) const)
{
	return ConstMemberCallbackFactory3<R, T, P1, P2, P3>();
}

// 4 parameter version

template<typename R, typename P1, typename P2, typename P3,
typename P4>
class TCallback<R (P1, P2, P3, P4)>
{
public:
	static const int Arity = 4;
	typedef R ReturnType;
	typedef P1 Param1Type;
	typedef P2 Param2Type;
	typedef P3 Param3Type;
	typedef P4 Param4Type;

	TCallback()                    : func(0), obj(0) {}
	TCallback(NullCallback)        : func(0), obj(0) {}
	TCallback(const TCallback& rhs) : func(rhs.func), obj(rhs.obj) {}
	~TCallback() {} 

	TCallback& operator=(NullCallback)
	{ obj = 0; func = 0; return *this; }
	TCallback& operator=(const TCallback& rhs)
	{ obj = rhs.obj; func = rhs.func; return *this; }

	inline R operator()(P1 a1, P2 a2, P3 a3, P4 a4) const
	{
		return (*func)(obj, a1, a2, a3, a4);
	}
	// for storing in containers
	inline bool operator == ( const TCallback& rhs ) const
	{
		return obj == rhs.obj
			&& func == rhs.func
			;
	}
	inline bool operator != ( const TCallback& rhs ) const
	{
		return !this->operator==(rhs);
	}
	template< class KLASS >
	inline const KLASS* ToClassPtr() const
	{
		return static_cast< const KLASS* >( obj );
	}
private:
	typedef const void* TCallback::*SafeBoolType;
public:
	inline operator SafeBoolType() const
	{ return func != 0 ? &TCallback::obj : 0; }
	inline bool operator!() const
	{ return func == 0; }

private:
	typedef R (*FuncType)(const void*, P1, P2, P3, P4);
	TCallback(FuncType f, const void* o) : func(f), obj(o) {}

private:
	FuncType func;
	const void* obj;

	template<typename FR, typename FP1, typename FP2, typename FP3,
		typename FP4>
		friend class FreeCallbackFactory4;
	template<typename FR, class FT, typename FP1, typename FP2, typename FP3,
		typename FP4>
		friend class MemberCallbackFactory4;
	template<typename FR, class FT, typename FP1, typename FP2, typename FP3,
		typename FP4>
		friend class ConstMemberCallbackFactory4;
};

template<typename R, typename P1, typename P2, typename P3,
typename P4>
void operator==(const TCallback<R (P1, P2, P3, P4)>&,
				const TCallback<R (P1, P2, P3, P4)>&);
template<typename R, typename P1, typename P2, typename P3,
typename P4>
void operator!=(const TCallback<R (P1, P2, P3, P4)>&,
				const TCallback<R (P1, P2, P3, P4)>&);

template<typename R, typename P1, typename P2, typename P3,
typename P4>
class FreeCallbackFactory4
{
private:
	template<R (*Func)(P1, P2, P3, P4)>
	static R Wrapper(const void*, P1 a1, P2 a2, P3 a3, P4 a4)
	{
		return (*Func)(a1, a2, a3, a4);
	}

public:
	template<R (*Func)(P1, P2, P3, P4)>
	inline static TCallback<R (P1, P2, P3, P4)> MakeCallback()
	{
		return TCallback<R (P1, P2, P3, P4)>
			(&FreeCallbackFactory4::Wrapper<Func>, nil);
	}
};

template<typename R, typename P1, typename P2, typename P3,
typename P4>
inline FreeCallbackFactory4<R, P1, P2, P3, P4>
GetCallbackFactory(R (*)(P1, P2, P3, P4))
{
	return FreeCallbackFactory4<R, P1, P2, P3, P4>();
}

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4>
class MemberCallbackFactory4
{
private:
	template<R (T::*Func)(P1, P2, P3, P4)>
	static R Wrapper(const void* o, P1 a1, P2 a2, P3 a3, P4 a4)
	{
		T* obj = const_cast<T*>(static_cast<const T*>(o));
		return (obj->*Func)(a1, a2, a3, a4);
	}

public:
	template<R (T::*Func)(P1, P2, P3, P4)>
	inline static TCallback<R (P1, P2, P3, P4)> MakeCallback(T& o)
	{
		return TCallback<R (P1, P2, P3, P4)>
			(&MemberCallbackFactory4::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4>
inline MemberCallbackFactory4<R, T, P1, P2, P3, P4>
GetCallbackFactory(R (T::*)(P1, P2, P3, P4))
{
	return MemberCallbackFactory4<R, T, P1, P2, P3, P4>();
}

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4>
class ConstMemberCallbackFactory4
{
private:
	template<R (T::*Func)(P1, P2, P3, P4) const>
	static R Wrapper(const void* o, P1 a1, P2 a2, P3 a3, P4 a4)
	{
		const T* obj = static_cast<const T*>(o);
		return (obj->*Func)(a1, a2, a3, a4);
	}

public:
	template<R (T::*Func)(P1, P2, P3, P4) const>
	inline static TCallback<R (P1, P2, P3, P4)> MakeCallback(const T& o)
	{
		return TCallback<R (P1, P2, P3, P4)>
			(&ConstMemberCallbackFactory4::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4>
inline ConstMemberCallbackFactory4<R, T, P1, P2, P3, P4>
GetCallbackFactory(R (T::*)(P1, P2, P3, P4) const)
{
	return ConstMemberCallbackFactory4<R, T, P1, P2, P3, P4>();
}

// 5 parameter version

template<typename R, typename P1, typename P2, typename P3,
typename P4, typename P5>
class TCallback<R (P1, P2, P3, P4, P5)>
{
public:
	static const int Arity = 5;
	typedef R ReturnType;
	typedef P1 Param1Type;
	typedef P2 Param2Type;
	typedef P3 Param3Type;
	typedef P4 Param4Type;
	typedef P5 Param5Type;

	TCallback()                    : func(0), obj(0) {}
	TCallback(NullCallback)        : func(0), obj(0) {}
	TCallback(const TCallback& rhs) : func(rhs.func), obj(rhs.obj) {}
	~TCallback() {} 

	TCallback& operator=(NullCallback)
	{ obj = 0; func = 0; return *this; }
	TCallback& operator=(const TCallback& rhs)
	{ obj = rhs.obj; func = rhs.func; return *this; }

	inline R operator()(P1 a1, P2 a2, P3 a3, P4 a4, P5 a5) const
	{
		return (*func)(obj, a1, a2, a3, a4, a5);
	}
	// for storing in containers
	inline bool operator == ( const TCallback& rhs ) const
	{
		return obj == rhs.obj
			&& func == rhs.func
			;
	}
	inline bool operator != ( const TCallback& rhs ) const
	{
		return !this->operator==(rhs);
	}
	template< class KLASS >
	inline const KLASS* ToClassPtr() const
	{
		return static_cast< const KLASS* >( obj );
	}
private:
	typedef const void* TCallback::*SafeBoolType;
public:
	inline operator SafeBoolType() const
	{ return func != 0 ? &TCallback::obj : 0; }
	inline bool operator!() const
	{ return func == 0; }

private:
	typedef R (*FuncType)(const void*, P1, P2, P3, P4, P5);
	TCallback(FuncType f, const void* o) : func(f), obj(o) {}

private:
	FuncType func;
	const void* obj;

	template<typename FR, typename FP1, typename FP2, typename FP3,
		typename FP4, typename FP5>
		friend class FreeCallbackFactory5;
	template<typename FR, class FT, typename FP1, typename FP2, typename FP3,
		typename FP4, typename FP5>
		friend class MemberCallbackFactory5;
	template<typename FR, class FT, typename FP1, typename FP2, typename FP3,
		typename FP4, typename FP5>
		friend class ConstMemberCallbackFactory5;
};

template<typename R, typename P1, typename P2, typename P3,
typename P4, typename P5>
void operator==(const TCallback<R (P1, P2, P3, P4, P5)>&,
				const TCallback<R (P1, P2, P3, P4, P5)>&);
template<typename R, typename P1, typename P2, typename P3,
typename P4, typename P5>
void operator!=(const TCallback<R (P1, P2, P3, P4, P5)>&,
				const TCallback<R (P1, P2, P3, P4, P5)>&);

template<typename R, typename P1, typename P2, typename P3,
typename P4, typename P5>
class FreeCallbackFactory5
{
private:
	template<R (*Func)(P1, P2, P3, P4, P5)>
	static R Wrapper(const void*, P1 a1, P2 a2, P3 a3, P4 a4, P5 a5)
	{
		return (*Func)(a1, a2, a3, a4, a5);
	}

public:
	template<R (*Func)(P1, P2, P3, P4, P5)>
	inline static TCallback<R (P1, P2, P3, P4, P5)> MakeCallback()
	{
		return TCallback<R (P1, P2, P3, P4, P5)>
			(&FreeCallbackFactory5::Wrapper<Func>, nil);
	}
};

template<typename R, typename P1, typename P2, typename P3,
typename P4, typename P5>
inline FreeCallbackFactory5<R, P1, P2, P3, P4, P5>
GetCallbackFactory(R (*)(P1, P2, P3, P4, P5))
{
	return FreeCallbackFactory5<R, P1, P2, P3, P4, P5>();
}

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4, typename P5>
class MemberCallbackFactory5
{
private:
	template<R (T::*Func)(P1, P2, P3, P4, P5)>
	static R Wrapper(const void* o, P1 a1, P2 a2, P3 a3, P4 a4, P5 a5)
	{
		T* obj = const_cast<T*>(static_cast<const T*>(o));
		return (obj->*Func)(a1, a2, a3, a4, a5);
	}

public:
	template<R (T::*Func)(P1, P2, P3, P4, P5)>
	inline static TCallback<R (P1, P2, P3, P4, P5)> MakeCallback(T& o)
	{
		return TCallback<R (P1, P2, P3, P4, P5)>
			(&MemberCallbackFactory5::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4, typename P5>
inline MemberCallbackFactory5<R, T, P1, P2, P3, P4, P5>
GetCallbackFactory(R (T::*)(P1, P2, P3, P4, P5))
{
	return MemberCallbackFactory5<R, T, P1, P2, P3, P4, P5>();
}

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4, typename P5>
class ConstMemberCallbackFactory5
{
private:
	template<R (T::*Func)(P1, P2, P3, P4, P5) const>
	static R Wrapper(const void* o, P1 a1, P2 a2, P3 a3, P4 a4, P5 a5)
	{
		const T* obj = static_cast<const T*>(o);
		return (obj->*Func)(a1, a2, a3, a4, a5);
	}

public:
	template<R (T::*Func)(P1, P2, P3, P4, P5) const>
	inline static TCallback<R (P1, P2, P3, P4, P5)> MakeCallback(const T& o)
	{
		return TCallback<R (P1, P2, P3, P4, P5)>
			(&ConstMemberCallbackFactory5::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4, typename P5>
inline ConstMemberCallbackFactory5<R, T, P1, P2, P3, P4, P5>
GetCallbackFactory(R (T::*)(P1, P2, P3, P4, P5) const)
{
	return ConstMemberCallbackFactory5<R, T, P1, P2, P3, P4, P5>();
}

// 6 parameter version

template<typename R, typename P1, typename P2, typename P3,
typename P4, typename P5, typename P6>
class TCallback<R (P1, P2, P3, P4, P5, P6)>
{
public:
	static const int Arity = 6;
	typedef R ReturnType;
	typedef P1 Param1Type;
	typedef P2 Param2Type;
	typedef P3 Param3Type;
	typedef P4 Param4Type;
	typedef P5 Param5Type;
	typedef P6 Param6Type;

	TCallback()                    : func(0), obj(0) {}
	TCallback(NullCallback)        : func(0), obj(0) {}
	TCallback(const TCallback& rhs) : func(rhs.func), obj(rhs.obj) {}
	~TCallback() {} 

	TCallback& operator=(NullCallback)
	{ obj = 0; func = 0; return *this; }
	TCallback& operator=(const TCallback& rhs)
	{ obj = rhs.obj; func = rhs.func; return *this; }

	inline R operator()(P1 a1, P2 a2, P3 a3, P4 a4, P5 a5, P6 a6) const
	{
		return (*func)(obj, a1, a2, a3, a4, a5, a6);
	}
	// for storing in containers
	inline bool operator == ( const TCallback& rhs ) const
	{
		return obj == rhs.obj
			&& func == rhs.func
			;
	}
	inline bool operator != ( const TCallback& rhs ) const
	{
		return !this->operator==(rhs);
	}
	template< class KLASS >
	inline const KLASS* ToClassPtr() const
	{
		return static_cast< const KLASS* >( obj );
	}
private:
	typedef const void* TCallback::*SafeBoolType;
public:
	inline operator SafeBoolType() const
	{ return func != 0 ? &TCallback::obj : 0; }
	inline bool operator!() const
	{ return func == 0; }

private:
	typedef R (*FuncType)(const void*, P1, P2, P3, P4, P5, P6);
	TCallback(FuncType f, const void* o) : func(f), obj(o) {}

private:
	FuncType func;
	const void* obj;

	template<typename FR, typename FP1, typename FP2, typename FP3,
		typename FP4, typename FP5, typename FP6>
		friend class FreeCallbackFactory6;
	template<typename FR, class FT, typename FP1, typename FP2, typename FP3,
		typename FP4, typename FP5, typename FP6>
		friend class MemberCallbackFactory6;
	template<typename FR, class FT, typename FP1, typename FP2, typename FP3,
		typename FP4, typename FP5, typename FP6>
		friend class ConstMemberCallbackFactory6;
};

template<typename R, typename P1, typename P2, typename P3,
typename P4, typename P5, typename P6>
void operator==(const TCallback<R (P1, P2, P3, P4, P5, P6)>&,
				const TCallback<R (P1, P2, P3, P4, P5, P6)>&);
template<typename R, typename P1, typename P2, typename P3,
typename P4, typename P5, typename P6>
void operator!=(const TCallback<R (P1, P2, P3, P4, P5, P6)>&,
				const TCallback<R (P1, P2, P3, P4, P5, P6)>&);

template<typename R, typename P1, typename P2, typename P3,
typename P4, typename P5, typename P6>
class FreeCallbackFactory6
{
private:
	template<R (*Func)(P1, P2, P3, P4, P5, P6)>
	static R Wrapper(const void*, P1 a1, P2 a2, P3 a3, P4 a4, P5 a5, P6 a6)
	{
		return (*Func)(a1, a2, a3, a4, a5, a6);
	}

public:
	template<R (*Func)(P1, P2, P3, P4, P5, P6)>
	inline static TCallback<R (P1, P2, P3, P4, P5, P6)> MakeCallback()
	{
		return TCallback<R (P1, P2, P3, P4, P5, P6)>
			(&FreeCallbackFactory6::Wrapper<Func>, nil);
	}
};

template<typename R, typename P1, typename P2, typename P3,
typename P4, typename P5, typename P6>
inline FreeCallbackFactory6<R, P1, P2, P3, P4, P5, P6>
GetCallbackFactory(R (*)(P1, P2, P3, P4, P5, P6))
{
	return FreeCallbackFactory6<R, P1, P2, P3, P4, P5, P6>();
}

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4, typename P5, typename P6>
class MemberCallbackFactory6
{
private:
	template<R (T::*Func)(P1, P2, P3, P4, P5, P6)>
	static R Wrapper(const void* o, P1 a1, P2 a2, P3 a3, P4 a4, P5 a5, P6 a6)
	{
		T* obj = const_cast<T*>(static_cast<const T*>(o));
		return (obj->*Func)(a1, a2, a3, a4, a5, a6);
	}

public:
	template<R (T::*Func)(P1, P2, P3, P4, P5, P6)>
	inline static TCallback<R (P1, P2, P3, P4, P5, P6)> MakeCallback(T& o)
	{
		return TCallback<R (P1, P2, P3, P4, P5, P6)>
			(&MemberCallbackFactory6::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4, typename P5, typename P6>
inline MemberCallbackFactory6<R, T, P1, P2, P3, P4, P5, P6>
GetCallbackFactory(R (T::*)(P1, P2, P3, P4, P5, P6))
{
	return MemberCallbackFactory6<R, T, P1, P2, P3, P4, P5, P6>();
}

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4, typename P5, typename P6>
class ConstMemberCallbackFactory6
{
private:
	template<R (T::*Func)(P1, P2, P3, P4, P5, P6) const>
	static R Wrapper(const void* o, P1 a1, P2 a2, P3 a3, P4 a4, P5 a5, P6 a6)
	{
		const T* obj = static_cast<const T*>(o);
		return (obj->*Func)(a1, a2, a3, a4, a5, a6);
	}

public:
	template<R (T::*Func)(P1, P2, P3, P4, P5, P6) const>
	inline static TCallback<R (P1, P2, P3, P4, P5, P6)> MakeCallback(const T& o)
	{
		return TCallback<R (P1, P2, P3, P4, P5, P6)>
			(&ConstMemberCallbackFactory6::Wrapper<Func>,
			static_cast<const void*>(&o));
	}
};

template<typename R, class T, typename P1, typename P2, typename P3,
typename P4, typename P5, typename P6>
inline ConstMemberCallbackFactory6<R, T, P1, P2, P3, P4, P5, P6>
GetCallbackFactory(R (T::*)(P1, P2, P3, P4, P5, P6) const)
{
	return ConstMemberCallbackFactory6<R, T, P1, P2, P3, P4, P5, P6>();
}



//---------------------------------------------------------------------------
// N = 0
// returns 'void' and accepts 'void'
//
template< typename RETURN_TYPE = void >
class TEvent0
{
public:
	typedef void ReturnType;
	typedef TCallback< ReturnType () >	CallbackType;

	TSmallList< CallbackType >	m_delegates;

public:

	template< class KLASS, void (KLASS::*FUNC)() >
	void Connect( KLASS& pObj, void (KLASS::*pFunc)() )
	{
		m_delegates.AddUnique(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}
	template< class KLASS, void (KLASS::*FUNC)() >
	void Disconnect( KLASS& pObj, void (KLASS::*pFunc)() )
	{
		m_delegates.Remove(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}

	template< class KLASS >
	void Disconnect( KLASS& pObj )
	{
		UINT numDelegates = m_delegates.Num();
		CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			const CallbackType& rDelegate = pDelegates[ iDelegate ];
			if( &pObj == rDelegate.ToClassPtr<KLASS>() )
			{
				m_delegates.RemoveAt_Fast( iDelegate );
				numDelegates--;
			}
		}
	}

	void Emit() const
	{
		const UINT numDelegates = m_delegates.Num();
		const CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			pDelegates[ iDelegate ] ();
		}
	}

	inline void operator() () const
	{
		Emit();
	}

	void Clear()
	{
		m_delegates.Clear();
	}
};

//---------------------------------------------------------------------------
// N = 1

template< typename P0 >
class TEvent1
{
public:
	typedef void ReturnType;
	typedef TCallback< ReturnType ( P0 ) >	CallbackType;

	TSmallList< CallbackType >	m_delegates;

public:

	template< class KLASS, void (KLASS::*FUNC)( P0 p0 ) >
	void Connect( KLASS& pObj, void (KLASS::*pFunc)( P0 p0 ) )
	{
		m_delegates.AddUnique(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}
	template< class KLASS, void (KLASS::*FUNC)( P0 p0 ) >
	void Disconnect( KLASS& pObj, void (KLASS::*pFunc)() )
	{
		m_delegates.Remove(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}

	template< class KLASS >
	void Disconnect( KLASS& pObj )
	{
		UINT numDelegates = m_delegates.Num();
		CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			const CallbackType& rDelegate = pDelegates[ iDelegate ];
			if( &pObj == rDelegate.ToClassPtr<KLASS>() )
			{
				m_delegates.RemoveAt_Fast( iDelegate );
				numDelegates--;
			}
		}
	}

	void Emit( P0 p0 ) const
	{
		const UINT numDelegates = m_delegates.Num();
		const CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			pDelegates[ iDelegate ] ( p0 );
		}
	}

	inline void operator() ( P0 p0 ) const
	{
		Emit( p0 );
	}

	void Clear()
	{
		m_delegates.Clear();
	}
};


//---------------------------------------------------------------------------
// N = 2

template< typename P0, typename P1 >
class TEvent2
{
public:
	typedef void ReturnType;
	typedef TCallback< ReturnType ( P0, P1 ) >	CallbackType;

	TSmallList< CallbackType >	m_delegates;

public:

	template< class KLASS, void (KLASS::*FUNC)( P0, P1 ) >
	void Connect( KLASS& pObj, void (KLASS::*pFunc)( P0, P1 ) )
	{
		m_delegates.AddUnique(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}
	template< class KLASS, void (KLASS::*FUNC)( P0, P1 ) >
	void Disconnect( KLASS& pObj, void (KLASS::*pFunc)() )
	{
		m_delegates.Remove(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}

	template< class KLASS >
	void Disconnect( KLASS& pObj )
	{
		UINT numDelegates = m_delegates.Num();
		CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			const CallbackType& rDelegate = pDelegates[ iDelegate ];
			if( &pObj == rDelegate.ToClassPtr<KLASS>() )
			{
				m_delegates.RemoveAt_Fast( iDelegate );
				numDelegates--;
			}
		}
	}

	void Emit( P0 p0, P1 p1 ) const
	{
		const UINT numDelegates = m_delegates.Num();
		const CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			pDelegates[ iDelegate ] ( p0, p1 );
		}
	}

	inline void operator() ( P0 p0, P1 p1 ) const
	{
		Emit( p0, p1 );
	}

	void Clear()
	{
		m_delegates.Clear();
	}
};


//---------------------------------------------------------------------------
// N = 3

template< typename P0, typename P1, typename P2 >
class TEvent3
{
public:
	typedef void ReturnType;
	typedef TCallback< ReturnType ( P0, P1, P2 ) >	CallbackType;

	TSmallList< CallbackType >	m_delegates;

public:

	template< class KLASS, void (KLASS::*FUNC)( P0, P1, P2 ) >
	void Connect( KLASS& pObj, void (KLASS::*pFunc)( P0, P1, P2 ) )
	{
		m_delegates.AddUnique(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}
	template< class KLASS, void (KLASS::*FUNC)( P0, P1, P2 ) >
	void Disconnect( KLASS& pObj, void (KLASS::*pFunc)() )
	{
		m_delegates.Remove(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}
	template< class KLASS >
	void Disconnect( KLASS& pObj )
	{
		UINT numDelegates = m_delegates.Num();
		CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			const CallbackType& rDelegate = pDelegates[ iDelegate ];
			if( &pObj == rDelegate.ToClassPtr<KLASS>() )
			{
				m_delegates.RemoveAt_Fast( iDelegate );
				numDelegates--;
			}
		}
	}

	void Emit( P0 p0, P1 p1, P2 p2 ) const
	{
		const UINT numDelegates = m_delegates.Num();
		const CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			pDelegates[ iDelegate ] ( p0, p1, p2 );
		}
	}

	inline void operator() ( P0 p0, P1 p1, P2 p2 ) const
	{
		Emit( p0, p1, p2 );
	}

	void Clear()
	{
		m_delegates.Clear();
	}
};


//---------------------------------------------------------------------------
// N = 4

template< typename P0, typename P1, typename P2, typename P3 >
class TEvent4
{
public:
	typedef void ReturnType;
	typedef TCallback< ReturnType ( P0, P1, P2, P3 ) >	CallbackType;

	TSmallList< CallbackType >	m_delegates;

public:

	template< class KLASS, void (KLASS::*FUNC)( P0, P1, P2, P3 ) >
	void Connect( KLASS& pObj, void (KLASS::*pFunc)( P0, P1, P2, P3 ) )
	{
		m_delegates.AddUnique(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}
	template< class KLASS, void (KLASS::*FUNC)( P0, P1, P2, P3 ) >
	void Disconnect( KLASS& pObj, void (KLASS::*pFunc)() )
	{
		m_delegates.Remove(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}

	template< class KLASS >
	void Disconnect( KLASS& pObj )
	{
		UINT numDelegates = m_delegates.Num();
		CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			const CallbackType& rDelegate = pDelegates[ iDelegate ];
			if( &pObj == rDelegate.ToClassPtr<KLASS>() )
			{
				m_delegates.RemoveAt_Fast( iDelegate );
				numDelegates--;
			}
		}
	}

	void Emit( P0 p0, P1 p1, P2 p2, P3 p3 ) const
	{
		const UINT numDelegates = m_delegates.Num();
		const CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			pDelegates[ iDelegate ] ( p0, p1, p2, p3 );
		}
	}

	inline void operator() ( P0 p0, P1 p1, P2 p2, P3 p3 ) const
	{
		Emit( p0, p1, p2, p3 );
	}

	void Clear()
	{
		m_delegates.Clear();
	}
};

//---------------------------------------------------------------------------
// N = 5

template< typename P0, typename P1, typename P2, typename P3, typename P4 >
class TEvent5
{
public:
	typedef void ReturnType;
	typedef TCallback< ReturnType ( P0, P1, P2, P3, P4 ) >	CallbackType;

	TSmallList< CallbackType >	m_delegates;

public:

	template< class KLASS, void (KLASS::*FUNC)( P0, P1, P2, P3, P4 ) >
	void Connect( KLASS& pObj, void (KLASS::*pFunc)( P0, P1, P2, P3, P4 ) )
	{
		m_delegates.AddUnique(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}
	template< class KLASS, void (KLASS::*FUNC)( P0, P1, P2, P3, P4 ) >
	void Disconnect( KLASS& pObj, void (KLASS::*pFunc)() )
	{
		m_delegates.Remove(
			GetCallbackFactory( pFunc ).MakeCallback< FUNC >( pObj )
		);
	}

	template< class KLASS >
	void Disconnect( KLASS& pObj )
	{
		UINT numDelegates = m_delegates.Num();
		CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			const CallbackType& rDelegate = pDelegates[ iDelegate ];
			if( &pObj == rDelegate.ToClassPtr<KLASS>() )
			{
				m_delegates.RemoveAt_Fast( iDelegate );
				numDelegates--;
			}
		}
	}

	void Emit( P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 ) const
	{
		const UINT numDelegates = m_delegates.Num();
		const CallbackType* pDelegates = m_delegates.ToPtr();

		for(UINT iDelegate = 0; iDelegate < numDelegates; iDelegate++)
		{
			pDelegates[ iDelegate ] ( p0, p1, p2, p3, p4 );
		}
	}

	inline void operator() ( P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 ) const
	{
		Emit( p0, p1, p2, p3, p4 );
	}

	void Clear()
	{
		m_delegates.Clear();
	}
};

//---------------------------------------------------------------------------

#define mxCONNECT_THIS( EVENT, CLASS, FUNCTION )\
	EVENT.Connect< CLASS, &CLASS::FUNCTION >( *this, &CLASS::FUNCTION )

// make sure to call mxDISCONNECT_THIS in destructor to prevent crashes
#define mxDISCONNECT_THIS( EVENT )\
	EVENT.Disconnect(*this);


// use for free functions and static functions of a class
#define mxBIND_FREE_FUNCTION( FUNCTION )\
	GetCallbackFactory( &FUNCTION ).MakeCallback< FUNCTION >();

// use for member functions of a class
#define mxBIND_MEMBER_FUNCTION( CLASS, FUNCTION, INSTANCE )\
	GetCallbackFactory( &CLASS::FUNCTION ).MakeCallback< &CLASS::FUNCTION >( INSTANCE );

//---------------------------------------------------------------------------


// NOTE: don't use this, there are better alternatives.
#if 0

mxSWIPED("Nebula3");
/*
	Nebula3 delegate class, allows to store a method call into a C++ object
	for later execution.

	See http://www.codeproject.com/KB/cpp/ImpossiblyFastCppDelegate.aspx
	for details.
*/

template<class ARGTYPE> class Delegate
{
public:
    /// constructor
    Delegate();
    /// invocation operator
    void operator()(ARGTYPE arg) const;
    /// setup a new delegate from a method call
    template<class CLASS, void (CLASS::*METHOD)(ARGTYPE)> static Delegate<ARGTYPE> FromMethod(CLASS* objPtr);
    /// setup a new delegate from a function call
    template<void(*FUNCTION)(ARGTYPE)> static Delegate<ARGTYPE> FromFunction();

private:
    /// method pointer typedef
    typedef void (*StubType)(void*, ARGTYPE);
    /// static method-call stub 
    template<class CLASS, void(CLASS::*METHOD)(ARGTYPE)> static void MethodStub(void* objPtr, ARGTYPE arg);
    /// static function-call stub
    template<void(*FUNCTION)(ARGTYPE)> static void FunctionStub(void* dummyPtr, ARGTYPE arg);

    void* objPtr;
    StubType stubPtr;
};

//------------------------------------------------------------------------------
/**
*/
template<class ARGTYPE>
Delegate<ARGTYPE>::Delegate() :
    objPtr(0),
    stubPtr(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class ARGTYPE> void
Delegate<ARGTYPE>::operator()(ARGTYPE arg) const
{
    (*this->stubPtr)(this->objPtr, arg);
}

//------------------------------------------------------------------------------
/**
*/
template<class ARGTYPE>
template<class CLASS, void (CLASS::*METHOD)(ARGTYPE)>
static Delegate<ARGTYPE>
Delegate<ARGTYPE>::FromMethod(CLASS* objPtr_)
{
    Delegate<ARGTYPE> del;
    del.objPtr = objPtr_;
    del.stubPtr = &MethodStub<CLASS,METHOD>;
    return del;
}

//------------------------------------------------------------------------------
/**
*/
template<class ARGTYPE>
template<void(*FUNCTION)(ARGTYPE)>
static Delegate<ARGTYPE>
Delegate<ARGTYPE>::FromFunction()
{
    Delegate<ARGTYPE> del;
    del.objPtr = 0;
    del.stubPtr = &FunctionStub<FUNCTION>;
    return del;
}

//------------------------------------------------------------------------------
/**
*/
template<class ARGTYPE>
template<class CLASS, void (CLASS::*METHOD)(ARGTYPE)>
static void
Delegate<ARGTYPE>::MethodStub(void* objPtr_, ARGTYPE arg_)
{
    CLASS* obj = static_cast<CLASS*>(objPtr_);
    (obj->*METHOD)(arg_);
}

//------------------------------------------------------------------------------
/**
*/
template<class ARGTYPE>
template<void(*FUNCTION)(ARGTYPE)>
static void
Delegate<ARGTYPE>::FunctionStub(void* dummyPtr, ARGTYPE arg_)
{
    (*FUNCTION)(arg_);
}
#endif


#endif // !__MX_TEMPLATES_DELEGATE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

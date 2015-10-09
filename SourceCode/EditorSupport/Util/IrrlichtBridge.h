/*
=============================================================================
	File:	IrrlichtBridge.h
	Desc:
=============================================================================
*/
#pragma once

#include "framework/irrlicht-1.7.2/include/irrlicht.h"
#pragma comment(lib, "Irrlicht.lib")

namespace irr
{

	// assumes that refcount starts at 1
	template< class KLASS >	// where KLASS : IReferenceCounted
	class RefPtr
	{
		KLASS *		m_ptr;

	public:
		RefPtr()
		{
			m_ptr = nil;
		}
		RefPtr( KLASS* refCounted )
		{
			m_ptr = refCounted;
			// don't grab()
		}
		RefPtr( const RefPtr& other )
		{
			m_ptr = other.m_ptr;
			if(PtrToBool( m_ptr )) {
				m_ptr->grab();
			}
		}
		~RefPtr()
		{
			if(PtrToBool( m_ptr )) {
				m_ptr->drop();
			}
		}

		// Implicit conversions.

		operator KLASS* () const
		{
			return m_ptr;
		}

		KLASS& operator * () const
		{
			Assert( m_ptr );
			return *m_ptr;
		}
		KLASS* operator -> () const
		{
			return m_ptr;
		}
		bool operator ! () const
		{
			//return m_ptr == nil;
			return !PtrToBool( m_ptr );
		}

		// Comparisons.

		FORCEINLINE bool operator == ( KLASS* o ) const				{return m_ptr == o;}
		FORCEINLINE bool operator != ( KLASS* o ) const				{return m_ptr != o;}
		FORCEINLINE bool operator == ( const RefPtr& other ) const	{return m_ptr == other.m_ptr;}
		FORCEINLINE bool operator != ( const RefPtr& other ) const	{return m_ptr != other.m_ptr;}

		// Assignment.

		RefPtr& operator = ( KLASS* o )
		{
			if( m_ptr != o )
			{
				if(PtrToBool( m_ptr )) {
					m_ptr->drop();
				}

				m_ptr = o;

				if(PtrToBool( o )) {
					o->grab();
				}
			}
			return *this;
		}
		RefPtr& operator = ( const RefPtr& other )
		{
			return *this = other.m_ptr;
		}
	};


	static FORCEINLINE
	const Vec3D ToMyVec3D( const core::vector3df& src )
	{
		return Vec3D( src.X, src.Y, src.Z );
	}

	static FORCEINLINE
	const core::vector3df FromMyVec3D( const Vec3D& src )
	{
		return core::vector3df( src.x, src.y, src.z );
	}

	static FORCEINLINE
	const Vec2D ToMyVec2D( const core::vector2df& src )
	{
		return Vec2D( src.X, src.Y );
	}

	static FORCEINLINE
	void CopyVec3D( Vec3D &dest, const core::vector3df& src )
	{
		dest.x = src.X;
		dest.y = src.Y;
		dest.z = src.Z;
	}

	static FORCEINLINE
	void CopyVec2D( Vec2D &dest, const core::vector2df& src )
	{
		dest.x = src.X;
		dest.y = src.Y;
	}

	static FORCEINLINE
	const AABB ToMyAABB( const core::aabbox3d<f32>& src )
	{
		return AABB( ToMyVec3D(src.MinEdge), ToMyVec3D(src.MaxEdge) );
	}

}//namespace irr

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

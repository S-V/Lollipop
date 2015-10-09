/*
=============================================================================
	File:	pxContactCache.h
	Desc:	Contact processing.
=============================================================================
*/

#ifndef __PX_CONTACT_CACHE_H__
#define __PX_CONTACT_CACHE_H__

//
//	pxContactCache
//
class pxContactCache
{
public:
	pxContactManifold**	manifolds;
	UINT numManifolds;
	UINT totalNumContacts;

public:
	PX_INLINE pxContactCache()
	{
		this->Clear();
	}

	PX_INLINE void Clear() {
		manifolds = nil;
		numManifolds = 0;
		totalNumContacts = 0;
	}

	PX_INLINE bool IsEmpty() const {
		return totalNumContacts == 0;
	}

private: PREVENT_COPY(pxContactCache);
};

#endif // !__PX_CONTACT_CACHE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

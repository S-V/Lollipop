/*
=============================================================================
	File:	Tweakable.h
	Desc:	Tweakable constants for debugging and rapid prototyping.
	See:

	http://blogs.msdn.com/b/shawnhar/archive/2009/05/01/motogp-tweakables.aspx

	http://www.gamedev.net/page/resources/_/technical/game-programming/tweakable-constants-r2731
	http://www.gamedev.net/topic/559658-tweakable-constants/

	https://mollyrocket.com/forums/viewtopic.php?p=3355
	https://mollyrocket.com/forums/viewtopic.php?t=556

	http://www.pouet.net/topic.php?which=7126&page=1&x=19&y=10

=============================================================================
*/

#ifndef __MX_TWEAKABLE_CONSTANTS_H__
#define __MX_TWEAKABLE_CONSTANTS_H__

#include <Base/Templates/Containers/HashMap/TPointerMap.h>
#include <Core/Editor.h>
#include <Core/Editor/EditableProperties.h>

//--------------------------------------------------
//	Definitions of useful macros.
//--------------------------------------------------

#if MX_EDITOR

	#define HOT_BOOL(x)\
		gCore.tweaks->Tweak_Bool( &x, #x, __FILE__, __LINE__ )

	#define HOT_FLOAT(x)\
		gCore.tweaks->Tweak_Float32( &x, #x, __FILE__, __LINE__ )

#else

	#define HOT_BOOL(x)
	#define HOT_FLOAT(x)

#endif // MX_EDITOR


/*
-----------------------------------------------------------------------------
	TweakUtil
-----------------------------------------------------------------------------
*/
class TweakUtil : public AEditable, SingleInstance<TweakUtil>
{
	TPointerMap< AEditableProperty::Ref >	m_varsByPtr;

public:
	mxDECLARE_CLASS_ALLOCATOR( HeapTemp, TweakUtil );

	TweakUtil();
	~TweakUtil();

	bool Tweak_Bool( bool * var, const char* expr, const char* file, int line );
	S4 TweakValue( S4 * var, const char* expr, const char* file, int line );
	F4 Tweak_Float32( F4 * var, const char* expr, const char* file, int line );

public:
	virtual UINT edNumRows() const override;
	virtual AEditable* edGetParent() override;
	virtual AEditable* edItemAt( UINT index ) override;
	virtual UINT edIndexOf( const AEditable* child ) const override;
	virtual const char* edToChars( UINT column ) const override;
	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;

};//TweakUtil


#endif // !__MX_TWEAKABLE_CONSTANTS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

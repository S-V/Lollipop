#include "Game_PCH.h"
#pragma hdrstop
#if 0

/*================================
	class mxGameWeapon
================================*/

mxGameWeapon::mxGameWeapon()
	: m_owner( NULL )
	, m_iFireDelay( 350 )
	, m_pCurrentSound( NULL )
{
	m_eStatus = EStatus::Ready;

	m_iLastAttackTime = 0;

	m_sounds.Zero();
	//m_sounds[ OnEquipping ] = GSoundWorld->CacheSound( PATH_TO_MEDIA "Sounds/Weapons/Equip_Rifle.ogg" );
}

mxGameWeapon::~mxGameWeapon()
{
}

void mxGameWeapon::SetOwner( mxActor* newOwner )
{
	m_owner = newOwner;
}

mxActor* mxGameWeapon::GetOwner()
{
	return m_owner;
}

const WString & mxGameWeapon::GetName() const {
	return m_name;
}

void mxGameWeapon::Equip()
{
	//GSoundWorld->Play2D( m_sounds[ OnEquipping ] );
	GSoundWorld->Play2D( PATH_TO_MEDIA "Sounds/Weapons/Equip_Rifle.ogg" );
}

bool mxGameWeapon::HasAnyAmmo() const
{
	return false;
}

void mxGameWeapon::PrimaryFire()
{
	Unreachable();
}

void mxGameWeapon::SecondaryFire()
{
	Unreachable();
}

void mxGameWeapon::AlternateFire( u32 mode )
{}

void mxGameWeapon::CeaseFire()
{
	if ( m_pCurrentSound ) {
		GSoundWorld->StopSound( m_pCurrentSound );
		m_pCurrentSound = NULL;
	}
}

void mxGameWeapon::DryFire()
{}

void mxGameWeapon::EndAttack()
{
	// Derived classes can override CeaseFire().
	CeaseFire();

	//if ( ::irrklang::ISound * sound = m_sounds[ Single ] )
	//{
	//	sound->stop();
	//}
	m_eStatus = EStatus::Ready;
}

void mxGameWeapon::Reload()
{}

void mxGameWeapon::BeginAttack()
{
	//if ( m_eStatus == Ready ) {
		m_iLastAttackTime = GWorld->GetTime();
	//}
	m_eStatus = EStatus::Firing;
}

/*================================
	class Weapon_Automatic
================================*/

Weapon_Automatic::Weapon_Automatic()
	: m_fireRate( 1 )
{}

void Weapon_Automatic::Think( const REAL fElapsedTime )
{
	if ( m_eStatus == EStatus::Firing )
	{
		// See if it's time for the next shot
		u32 currTime = GWorld->GetTime();

		if ( currTime - m_iLastShotTime >= (1000 / m_fireRate) )
		{
			GSoundWorld->Play2D( m_sounds[ Single ] );
			m_iLastShotTime = currTime;

			// Do actual shooting.
			PrimaryFire();
		}
	}
}
#endif

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

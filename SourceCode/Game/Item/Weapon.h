/*
=============================================================================
	File:	GameItem.h
	Desc:	Item system.
=============================================================================
*/
#pragma once

#if 0

//
//	mxGameWeapon
//
class mxGameWeapon : public mxActor {
public:
	enum EStatus
	{
		Ready,
		Firing,
		OutOfAmmo,
		Reloading,
		Holstered,
		Rising,
		Lowering,
		Flashlight
	};

				//mxGameWeapon( mxActor& owner );
				mxGameWeapon();
				~mxGameWeapon();

	void	SetOwner( mxActor* newOwner );
	mxActor *	GetOwner();

	const WString &	GetName() const;

	// Weapon selection ------------------------------------------
	virtual void	Equip();
	virtual bool	HasAnyAmmo() const;

	// Weapon firing ---------------------------------------------

	// Derived classes are expected to override these:
	virtual void	PrimaryFire();
	virtual void	SecondaryFire();
	virtual void	AlternateFire( u32 mode );
	virtual void	CeaseFire();
	virtual void	DryFire();

			void	EndAttack();

	// Reloading -------------------------------------------------
	virtual void	Reload();

	//------------------------------------------------------------

	static mxGameWeapon *	GetNullWeapon();

protected:
	// Derived classes should call BeginAttack() before firing.
	void	BeginAttack();

protected:
	EStatus		m_eStatus;	// current status
	WString		m_name;		// The weapon�s name displayed in-game.

	mxActor *		m_owner;	// weapons can fire even if it's NULL

	u32			m_iLastAttackTime;

	// The period ( in milliseconds ) between two successive ammo firing.
	const u32	m_iFireDelay;

	//--- Weapon sounds. -------------------------------------------------------
	enum ESound
	{
		Empty,		NPC_Empty,
		Single,		NPC_Single,
		Double,		NPC_Double,
		Burst,		NPC_Burst,
		ReloadSnd,	NPC_ReloadSnd,
		Melee,

		//TODO: remove this, or generate ricochet sounds based on materials hit by bullets and rockets
		Ricochet,

		OnEquipping,

		NumOfWeaponSoundTypes	// Maximum number of weapon sounds per shoot type.
	};
	TFixedArray< mxSoundSource*, NumOfWeaponSoundTypes >		m_sounds;
	mxSound *		m_pCurrentSound;
};

//
//	Weapon_Automatic - a weapon which fires at discrete time intervals.
//
class Weapon_Automatic : public mxGameWeapon {
public:
			Weapon_Automatic();

	// override ( mxEntity )
	void	Think( const REAL fElapsedTime );

protected:
	u32		m_fireRate;	// shots per second

	u32		m_iLastShotTime;
};
#endif

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

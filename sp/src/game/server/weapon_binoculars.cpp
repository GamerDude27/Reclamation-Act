//========= Copyright Reclamation Act Development Team, All rights reserved. ============//
// 
// Purpose: Implements binoculars. 
// 
// TODO: Black mask around view when zoomed in.
// TODO: New reticle & hud effects.
// TODO: Animated zoom effect?
// 
// $NoKeywords: $ 
//========================================================================================//

#include "cbase.h" 
#include "NPCEvent.h" 
#include "basehlcombatweapon.h" 
#include "basecombatcharacter.h" 
#include "AI_BaseNPC.h" 
#include "player.h" 
#include "gamerules.h"
#include "entitylist.h" 
#include "soundenvelope.h" 
#include "in_buttons.h" 
#include "engine/IEngineSound.h"

class CWeaponBinoculars : public CBaseHLCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponBinoculars, CBaseHLCombatWeapon );

	bool Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	void ItemPostFrame( void );
	void ItemBusyFrame( void );

	DECLARE_SERVERCLASS();
	//DECLARE_DATADESC();

protected:
	void ToggleZoom( void );
	void CheckZoomToggle( void );

	bool m_bInZoom;
};

IMPLEMENT_SERVERCLASS_ST(CWeaponBinoculars, DT_WeaponBinoculars)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_binoculars, CWeaponBinoculars);

PRECACHE_WEAPON_REGISTER(weapon_binoculars);

//-----------------------------------------------------------------------------
// Purpose: Check for weapon being holstered so we can disable binoculars zoom
//-----------------------------------------------------------------------------
bool CWeaponBinoculars::Holster(CBaseCombatWeapon *pSwitchingTo /* = NULL  */)
{
	if (m_bInZoom)
	{
		ToggleZoom();
	}

	return BaseClass::Holster(pSwitchingTo);
}

//-----------------------------------------------------------------------------
// Purpose: Check the status of the zoom key every frame to see if player is still zoomed in
//-----------------------------------------------------------------------------
void CWeaponBinoculars::ItemPostFrame()
{
	// Allow zoom toggling
	CheckZoomToggle();

	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose: Check the status of the zoom key every frame to see if player is still zoomed in
//-----------------------------------------------------------------------------
void CWeaponBinoculars::ItemBusyFrame(void)
{
	// Allow zoom toggling even when busy
	CheckZoomToggle();

	BaseClass::ItemBusyFrame();
}

//-----------------------------------------------------------------------------
// Purpose: Check if the zoom key was pressed in the last input tick
//-----------------------------------------------------------------------------
void CWeaponBinoculars::CheckZoomToggle(void)
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer && (pPlayer->m_afButtonPressed & IN_ATTACK2))
	{
		ToggleZoom();
	}
}

//----------------------------------------------------------------------------- 
// Purpose: If we're zooming, stop. If we're not, start.
//----------------------------------------------------------------------------- 
void CWeaponBinoculars::ToggleZoom(void)
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

#ifndef CLIENT_DLL
	if (m_bInZoom)
	{
		if (pPlayer->SetFOV(this, 0, 0.2f))
		{
			m_bInZoom = false;

			// Send a message to hide the binoculars overlay
			CSingleUserRecipientFilter filter(pPlayer);
			UserMessageBegin(filter, "ShowBinoculars");
				WRITE_BYTE(0);
			MessageEnd();
		}
	}
	else
	{
		if (pPlayer->SetFOV(this, 20, 0.1f))
		{
			m_bInZoom = true;

			// Send a message to show the binoculars overlay
			CSingleUserRecipientFilter filter(pPlayer);
			UserMessageBegin(filter, "ShowBinoculars");
				WRITE_BYTE(1);
			MessageEnd();
		}
	}
#endif
}
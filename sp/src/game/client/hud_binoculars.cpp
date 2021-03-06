#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "c_basehlplayer.h" //alternative #include "c_baseplayer.h"

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Panel.h>

// memdbgon must be the last include file in a .cpp file!
#include "tier0/memdbgon.h"

/**
* Simple HUD element for displaying binoculars overlay on screen
*/
class CHudBinoculars : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CHudBinoculars, vgui::Panel);

public:
	CHudBinoculars(const char *pElementName);

	void Init();
	void MsgFunc_ShowBinoculars(bf_read &msg);

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void Paint(void);

private:
	bool			m_bShow;
	CHudTexture*	m_pBinoculars;

	char*			binoculars_src[4];
	int				m_iRes;
};

DECLARE_HUDELEMENT(CHudBinoculars);
DECLARE_HUD_MESSAGE(CHudBinoculars, ShowBinoculars);

using namespace vgui;

/**
* Constructor - generic HUD element initialization stuff. Make sure our 2 member variables
* are instantiated.
*/
CHudBinoculars::CHudBinoculars(const char *pElementName) : CHudElement(pElementName), BaseClass(NULL, "HudBinoculars")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	m_bShow = false;
	m_pBinoculars = 0;

	// Binoculars will not show when the player is dead
	SetHiddenBits(HIDEHUD_PLAYERDEAD);

	// fix for users with diffrent screen ratio (Lodle)
	int screenWide, screenTall;
	GetHudSize(screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);

	binoculars_src[0] = "binoculars43";
	binoculars_src[1] = "binoculars169";
	binoculars_src[2] = "binoculars32";
	binoculars_src[3] = "binoculars1610";
}

/**
* Hook up our HUD message, and make sure we are not showing the binoculars
*/
void CHudBinoculars::Init()
{
	HOOK_HUD_MESSAGE(CHudBinoculars, ShowBinoculars);

	m_bShow = false;
}

/**
* Load  in the binoculars material here
*/
void CHudBinoculars::ApplySchemeSettings(vgui::IScheme *scheme)
{
	BaseClass::ApplySchemeSettings(scheme);

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
}

/**
* Simple - if we want to show the binoculars, draw it. Otherwise don't.
*/
void CHudBinoculars::Paint(void)
{
	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
	{
		return;
	}

	if (m_bShow)
	{

		vgui::Panel *pParent = g_pClientMode->GetViewport();
		int screenWide = pParent->GetWide();
		int screenTall = pParent->GetTall();

		float ratio = ((float)screenWide) / ((float)screenTall);

		if (ratio == 1.60000000)
		{
			m_iRes = 3;
		}
		else if (ratio >= 1.59 && ratio <= 1.61)
		{
			m_iRes = 2;
		}
		else if (ratio >= 1.76 && ratio <= 1.78)
		{
			m_iRes = 1;
		}
		else
		{
			m_iRes = 0;
		}

		m_pBinoculars = gHUD.GetIcon(binoculars_src[m_iRes]);
		if (m_pBinoculars)
		{
			//Performing depth hack to prevent clips by world
			//materials->DepthRange(0.0f, 0.1f);

			m_pBinoculars->DrawSelf(0, 0, pParent->GetWide(), pParent->GetTall(), Color(255, 255, 255, 255));

			//Restore depth
			//materials->DepthRange(0.0f, 1.0f);

			// Hide the crosshair
			pPlayer->m_Local.m_iHideHUD |= HIDEHUD_CROSSHAIR;
		}

	}
	else if ((pPlayer->m_Local.m_iHideHUD & HIDEHUD_CROSSHAIR) != 0)
	{
		pPlayer->m_Local.m_iHideHUD &= ~HIDEHUD_CROSSHAIR;
	}
}

/**
* Callback for our message - set the show variable to whatever
* boolean value is received in the message
*/
void CHudBinoculars::MsgFunc_ShowBinoculars(bf_read &msg)
{
	m_bShow = msg.ReadByte();
}
#pragma once

namespace jew
{
	class settings_c
	{
	public:

		bool bBox = false;
		bool bSkeleton = false;
		bool bPenisaim = false;
		bool bDemoSpeed = false;
		bool bDistance = false;
		bool bPlayerName = false;
		bool bTracers = false;
		bool bWaterMark = false;
		bool bGun = false;
		bool bTeam = false;
		bool bShowMenu = false;
		bool bSkid = false;
		bool bDebug = false;
		bool bBulletTeleport = false;
		bool bNoEquipAnimation = false;
		bool bFOVChanger = false;
		bool bVis = true;
		float fFOVChangerValue = 100;
		bool bReloadWhileAiming = false;
		bool bAutoWeapons = false;
		int iMaxDistance = 280;
		bool bSkipKnocked = false;
		bool bEditEnemyBuilds = false;
		bool bNoRecoil = false;
		bool bSAIM = false;
		bool Memoryaim = false;
		bool bNoReload = false;
		bool bOnbro = false;
		bool bFa = false;
		bool bStorm = false;
		bool bAimbot = false;
		bool bPrediction = false;
		bool bNoSpread = false;
		int iAimbotKey = VK_MENU;
		int iMenukey = VK_SHIFT;

		float DemoSpeedValue = 5.f;
		float fAimFOV = 50.f;
		float fSmooth = 2.f;
	};

	inline settings_c settings;
}

class menu_c {
public:
	static void DrawMenu(SDK::UCanvas* Canvas, bool ShowMenu);

};
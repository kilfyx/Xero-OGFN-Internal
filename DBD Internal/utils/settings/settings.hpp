#pragma once

namespace jew
{
	class settings_c
	{
	public:

		bool bBox = false;
		bool bSkeleton = true;
		bool bDistance = true;
		bool bPlayerName = true;
		bool bTracers = true;
		bool bWaterMark = true;
		bool bGun = false;
		bool bTeam = false;
		bool bShowMenu = false;
		bool bSkid = true;
		bool bDebug = false;
		bool bBulletTeleport = false;
		bool bNoEquipAnimation = false;
		bool bFOVChanger = false;
		float fFOVChangerValue = 100;
		bool bReloadWhileAiming = false;
		bool bAutoWeapons = false;
		int iMaxDistance = 280;
		bool bSkipKnocked = true;
		bool bEditEnemyBuilds = false;
		bool bNoRecoil = false;
		bool bSAIM = false;
		bool bNoReload = false;
		bool bOnbro = false;
		bool bFa = false;
		bool bStorm = false;
		bool bAimbot = true;
		bool bPrediction = true;
		bool bNoSpread = false;
		int iAimbotKey = VK_SHIFT;

		float fAimFOV = 50.f;
		float fSmooth = 2.f;
	};

	inline settings_c settings;
}

class menu_c {
public:
	static void DrawMenu(SDK::UCanvas* Canvas, bool ShowMenu);
};
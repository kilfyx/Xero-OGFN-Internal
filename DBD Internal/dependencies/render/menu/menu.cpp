#pragma once
#include <dependencies/render/render.hpp>
#include <utils/settings/settings.hpp>
#include <dependencies/render/menu/menu.hpp>

auto jew::menu_c::DrawMenu(UCanvas* Canvas, bool ShowMenu) -> void
{
	ZeroGUI::SetupCanvas(Canvas);
	ZeroGUI::Input::Handle();

	static FVector2D Position = FVector2D(700, 700);


	float Width = ((double)Canvas->ClipX);
	float Height = ((double)Canvas->ClipY);

	if (ZeroGUI::Window("Lil tay onlyfans", &Position, FVector2D{ 350.0f, 400.0f }, ShowMenu))
	{
		static int CurrentTab = 0;

		if (ZeroGUI::ButtonTab("Combat", FVector2D{ 110, 25 }, CurrentTab == 0)) CurrentTab = 0;
		ZeroGUI::SameLine();
		if (ZeroGUI::ButtonTab("Visuals", FVector2D{ 110, 25 }, CurrentTab == 1)) CurrentTab = 1;
		ZeroGUI::SameLine();
		if (ZeroGUI::ButtonTab("Misc", FVector2D{ 110, 25 }, CurrentTab == 2)) CurrentTab = 2;

		if (CurrentTab == 0)
		{
			ZeroGUI::Checkbox("Aimbot", &jew::settings.bAimbot);
			ZeroGUI::Checkbox("Skip Knocked", &jew::settings.bSkipKnocked);
			ZeroGUI::Checkbox("Fov Circle", &jew::settings.bSkid);
			ZeroGUI::Checkbox("Prediction", &jew::settings.bPrediction);

			ZeroGUI::SliderFloat("Smoothing", &jew::settings.fSmooth, 1, 10);
			ZeroGUI::SliderFloat("Field Of View", &jew::settings.fAimFOV, 1, 300);

			FVector2D Size = FVector2D{ 150, 25 };
			ZeroGUI::Text("Key");
			ZeroGUI::Hotkey(" ", Size, &jew::settings.iAimbotKey);
		}

		if (CurrentTab == 1)
		{
			ZeroGUI::Checkbox("Skeleton", &jew::settings.bSkeleton);
			ZeroGUI::Checkbox("Fov Arrows", &jew::settings.bFa);
			ZeroGUI::Checkbox("Filled Box", &jew::settings.bSAIM);
			ZeroGUI::Checkbox("Gender Esp", &jew::settings.bOnbro);
			ZeroGUI::Checkbox("Box", &jew::settings.bBox);
			ZeroGUI::Checkbox("Name", &jew::settings.bPlayerName);
			ZeroGUI::Checkbox("Distance", &jew::settings.bDistance);
			ZeroGUI::Checkbox("Weapon", &jew::settings.bGun);
			ZeroGUI::Checkbox("Tracers", &jew::settings.bTracers);
		}

		if (CurrentTab == 2)
		{
			ZeroGUI::Checkbox("Debug", &jew::settings.bDebug);
			ZeroGUI::Checkbox("Peformance Mode (Doesnt Disable)", &jew::settings.bStorm);
			ZeroGUI::Text("i shot pdiddy");

		}
	}
}

// imgui, exploits
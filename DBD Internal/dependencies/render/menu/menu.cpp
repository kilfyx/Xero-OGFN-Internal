#pragma once
#include <dependencies/render/render.hpp>
#include <utils/settings/settings.hpp>
#include <dependencies/render/menu/menu.hpp>

auto jew::menu_c::DrawMenu(UCanvas* Canvas, bool ShowMenu) -> void
{
    ZeroGUI::SetupCanvas(Canvas);
    ZeroGUI::Input::Handle();

    static FVector2D Position = FVector2D(650, 400);

    float Width = Canvas->ClipX;
    float Height = Canvas->ClipY;

    if (ZeroGUI::Window("Xero OGFN", &Position, FVector2D{ 600.0f, 540.0f }, ShowMenu))
    {
        static int CurrentTab = 0;

        if (ZeroGUI::ButtonTab("Combat", FVector2D{ 590, 40 }, CurrentTab == 0)) CurrentTab = 0;
        if (ZeroGUI::ButtonTab("Visuals", FVector2D{ 590, 40 }, CurrentTab == 1)) CurrentTab = 1;
        if (ZeroGUI::ButtonTab("Misc", FVector2D{ 590, 40 }, CurrentTab == 2)) CurrentTab = 2;
 

        if (CurrentTab == 0)
        {
            ZeroGUI::Checkbox("Mouse Aimbot", &jew::settings.bAimbot);
            ZeroGUI::Checkbox("Memory Aimbot (not bothered to work on)", &jew::settings.bEditEnemyBuilds);
            ZeroGUI::Checkbox("Skip Knocked", &jew::settings.bSkipKnocked);
            ZeroGUI::Checkbox("Vis Check", &jew::settings.bVischeck);
            ZeroGUI::Checkbox("Fov Circle", &jew::settings.bSkid);
            ZeroGUI::Checkbox("Prediction", &jew::settings.bPrediction);

            ZeroGUI::SliderFloat("Smoothing", &jew::settings.fSmooth, 0, 5);
            ZeroGUI::SliderFloat("Field Of View", &jew::settings.fAimFOV, 1, 300);

            ZeroGUI::Text("Key");
            ZeroGUI::Hotkey(" ", FVector2D{ 150, 30 }, &jew::settings.iAimbotKey);
        }

        if (CurrentTab == 1)
        {
            ZeroGUI::Checkbox("Skeleton", &jew::settings.bSkeleton);
            ZeroGUI::Checkbox("Fov Arrows", &jew::settings.bFa);
            ZeroGUI::Checkbox("Filled Box", &jew::settings.bSAIM);
            ZeroGUI::Checkbox("Box", &jew::settings.bBox);
            ZeroGUI::Checkbox("Name", &jew::settings.bPlayerName);
            ZeroGUI::Checkbox("Distance", &jew::settings.bDistance);
            ZeroGUI::Checkbox("Weapon", &jew::settings.bGun);
            ZeroGUI::Checkbox("Tracers", &jew::settings.bTracers);
        }

        if (CurrentTab == 2)
        {
            ZeroGUI::Checkbox("Debug", &jew::settings.bDebug);
            ZeroGUI::Checkbox("Magic Bullet", &jew::settings.bBulletTeleport);
            ZeroGUI::Checkbox("Peformance Mode (Doesnt Disable)", &jew::settings.bStorm);
            ZeroGUI::Text("i shot pdiddy");
        } 
    }
}

#pragma once
#include <core/game/utils/classes/classes.hpp>
#include <dependencies/includes/includes.hpp>
#include <xor.h>
#include <cmath>
#include <numbers>

AFortPlayerPawn* ClosestPlayer = 0;

typedef void (*PostRender_t)(SDK::UObject* pObject, SDK::UCanvas* pCanvas);
static inline PostRender_t DrawTransitionOriginal;

using namespace SDK;

namespace SilentVariables
{
	FRotator TargetRotationWithSmooth;
	FRotator CameraRotation;
	FVector CameraLocation;
}

void PostRender(SDK::UGameViewportClient* Viewport, SDK::UCanvas* Canvas)
{
	if (!Viewport || !Canvas) return;
	UE->CachedCanvas = Canvas;
	static bool bHasSetupShit = false;

	if (!bHasSetupShit) { UE->FontSize = UE->DrawingFont->LegacyFontSize; bHasSetupShit = true; }

	UE->ScreenSize = FVector2D(Canvas->SizeX, Canvas->SizeY);
	UE->ScreenCenter = FVector2D(Canvas->SizeX / 2, Canvas->SizeY / 2);

	UWorld* GWorld = Viewport->World;
	if (!GWorld) return;

	UE->World = GWorld;

	UE->OwningGameInstance = GWorld->OwningGameInstance;
	if (!UE->OwningGameInstance) return;

	UE->LocalPlayer = UE->World->OwningGameInstance->LocalPlayers[0];
	if (!UE->LocalPlayer) return;

	auto PlayerController = UE->LocalPlayer->PlayerController;
	if (!PlayerController) return;
	UE->PlayerController = PlayerController;


	if (PlayerController->WasInputKeyJustPressed(UE->Insert))
	{
		jew::settings.bShowMenu = !jew::settings.bShowMenu;

	}

	jew::menu.DrawMenu(Canvas, jew::settings.bShowMenu);

	auto PlayerCameraManager = PlayerController->PlayerCameraManager;
	if (!PlayerCameraManager) return;

	auto AcknowledgedPawn = reinterpret_cast<AFortPlayerPawn*>(UE->World->OwningGameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn);

	if (AcknowledgedPawn)
		jew::weaponstats.WeaponTick(AcknowledgedPawn->CurrentWeapon);

	auto CameraLocation = PlayerCameraManager->GetCameraLocation();
	auto CameraRotation = PlayerCameraManager->GetCameraRotation();

	static float ClosestDistanceToCenter = FLT_MAX;

	SDK::TArray<SDK::AActor*> Players;


	if (jew::settings.bStorm)
	{
		if (UE->PlayerController && UE->World)
		{
			UGameInstance* GameInstance = UE->World->OwningGameInstance;
			if (GameInstance && GameInstance->LocalPlayers.Num() > 0)
			{
				UGameViewportClient* ViewportClient = GameInstance->LocalPlayers[0]->ViewportClient;
				if (ViewportClient)
				{
					*(uintptr_t*)((uintptr_t)ViewportClient + 0xB0) = 1;
				}
			}
		}
	}


	UE->GameplayStatics->GetAllActorsOfClass(UE->World, AFortPlayerPawn::StaticClass(), &Players);

	ClosestPlayer = nullptr;
	ClosestDistanceToCenter = FLT_MAX;

	for (int i = 0; i < Players.Num(); i++)
	{
		if (!Players.IsValidIndex(i)) continue;
		auto Player = (SDK::AFortPlayerPawn*)Players[i];
		if (!Player) continue;

		auto Mesh = Player->Mesh; if (!Mesh) continue;
		auto PlayerState = reinterpret_cast<SDK::AFortPlayerPawn*>(Player->PlayerState); if (!PlayerState) continue;
		auto RootComponent = Player->RootComponent; if (!RootComponent) continue;

		if (Player == AcknowledgedPawn)
			continue;

		bool IsVisible = Player->WasRecentlyRendered(0.f);

		auto HeadBone = Mesh->GetSocketLocation(UE->KismetStringLibrary->Conv_StringToName(L"head"));

		auto RootBone = Mesh->GetSocketLocation(UE->KismetStringLibrary->Conv_StringToName(L"root"));

		auto vHeadScreen = Utils::Project(FVector(HeadBone.X, HeadBone.Y, HeadBone.Z + 15), Canvas);

		auto vRootScreen = Utils::Project(RootBone, Canvas);

		float BoxHeight = (float)(vRootScreen.Y - vHeadScreen.Y);
		float BoxWidth = BoxHeight * 0.40f;

		float BottomLeftX = (float)vHeadScreen.X - BoxWidth / 2;
		float BottomLeftY = (float)vRootScreen.Y;

		float BottomRightX = (float)vHeadScreen.X + BoxWidth / 2;
		float BottomRightY = (float)vRootScreen.Y;

		float TopRightX = (float)vHeadScreen.X + BoxWidth / 2;
		float TopRightY = (float)vHeadScreen.Y;

		float TopLeftX = (float)vHeadScreen.X - BoxWidth / 2;
		float TopLeftY = (float)vHeadScreen.Y;

		FLinearColor iColor = IsVisible ? FLinearColor(1.0f, 1.0f, 1.0f, 1.0f) : FLinearColor(0.5f, 0.0f, 0.5f, 1.0f);

		if (jew::settings.bPlayerName)
		{
			auto PlayerName = Player->PlayerState->GetPlayerName();

			Canvas->K2_DrawText(UE->DrawingFont, PlayerName, FVector2D(vHeadScreen.X, vHeadScreen.Y - 15), FVector2D(1.0, 1.0), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), 0, FLinearColor(0.0f, 0.f, 0.f, 0.f), FVector2D(0, 0), true, false, true, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
		}


		if (jew::settings.bDistance)
		{
			float Distance = Utils::Vector_Distance(Player->K2_GetActorLocation(), CameraLocation) * 0.01;

			auto DistanceText = UE->KismetStringLibrary->BuildString_Float(L"", L"", UE->KismetMathLibrary->Round(Distance), L"m");

			Canvas->K2_DrawText(UE->DrawingFont, DistanceText, FVector2D(vRootScreen.X, vRootScreen.Y + 10), FVector2D(1.0, 1.0), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), 0, FLinearColor(0.0f, 0.f, 0.f, 0.f), FVector2D(0, 0), true, false, true, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
		}

		if (jew::settings.bGun)
		{
			FString WeaponName = _(L"No Item");
			SDK::FLinearColor RenderColor = SDK::FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

			if (Player->CurrentWeapon)
			{
				auto DisplayName = Player->CurrentWeapon->WeaponData->DisplayName;
				auto Tier = Player->CurrentWeapon->WeaponData->Tier;
				RenderColor = Utils::GetColorByTier(Tier);
				WeaponName = UKismetTextLibrary::Conv_TextToString(DisplayName);
			}

			Canvas->K2_DrawText(UE->DrawingFont, WeaponName, FVector2D(vRootScreen.X, vRootScreen.Y + 25), FVector2D(1.0, 1.0), RenderColor, 0, FLinearColor(0.0f, 0.f, 0.f, 0.f), FVector2D(0, 0), true, false, true, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
		}

		if (jew::settings.bOnbro)
		{

			enum EFortCustomGender : uint8_t {
				Invalid = 0,
				Male = 1,
				Female = 2,
				Both = 3
			};

			auto PlayerState = Player->PlayerState;
			if (!PlayerState) return;

			uint8_t gender = *(uint8_t*)((uintptr_t)PlayerState + 0x5D8);

			const char* genderStr = "???";
			if (gender == Male)    genderStr = "Male";
			else if (gender == Female)  genderStr = "Female";
			else if (gender == Both)    genderStr = "Trans/Bi";
			else if (gender == Invalid) genderStr = "???";

			Canvas->K2_DrawText(
				UE->DrawingFont,
				FString(genderStr),
				FVector2D(vHeadScreen.X, vHeadScreen.Y + 25),
				FVector2D(1.0f, 1.0f),
				FLinearColor(1.0f, 1.0f, 1.0f, 1.0f),
				0.0f,
				FLinearColor(0.0f, 0.0f, 0.0f, 1.0f),
				FVector2D(0.0f, 0.0f),
				true, false, true,
				FLinearColor(0.0f, 0.0f, 0.0f, 1.0f)
			);
		}


		if (jew::settings.bSkeleton)
		{
			const std::pair<const wchar_t*, const wchar_t*> BonePairs[] =
			{
				{L"head", L"neck_01"},
				{L"neck_01", L"upperarm_r"},
				{L"neck_01", L"upperarm_l"},
				{L"upperarm_l", L"lowerarm_l"},
				{L"lowerarm_l", L"hand_l"},
				{L"upperarm_r", L"lowerarm_r"},
				{L"lowerarm_r", L"hand_r"},
				{L"neck_01", L"pelvis"},
				{L"pelvis", L"thigh_r"},
				{L"pelvis", L"thigh_l"},
				{L"thigh_r", L"calf_r"},
				{L"calf_r", L"ik_foot_r"},
				{L"thigh_l", L"calf_l"},
				{L"calf_l", L"ik_foot_l"}
			};

			const size_t Count = sizeof(BonePairs) / sizeof(BonePairs[0]);

			for (size_t i = 0; i < Count; ++i)
			{
				FVector2D FirstBone2D = Utils::Project(Mesh->GetSocketLocation(UE->KismetStringLibrary->Conv_StringToName(BonePairs[i].first)), Canvas);
				FVector2D SecondBone2D = Utils::Project(Mesh->GetSocketLocation(UE->KismetStringLibrary->Conv_StringToName(BonePairs[i].second)), Canvas);

				ZeroGUI::Draw_Line(FirstBone2D, SecondBone2D, 1.3f, iColor);
			}
		}

		if (jew::settings.bTracers)
		{
			ZeroGUI::Draw_Line(UE->ScreenCenter, vRootScreen, 1.7f, iColor);
		}

		if (jew::settings.bBox)
		{
			if (IsVisible)
			{
				ZeroGUI::Draw_Line(FVector2D(BottomLeftX, BottomLeftY), FVector2D(BottomRightX, BottomRightY), 2.f, iColor);
				ZeroGUI::Draw_Line(FVector2D(BottomRightX, BottomRightY), FVector2D(TopRightX, TopRightY), 2.f, iColor);
				ZeroGUI::Draw_Line(FVector2D(TopRightX, TopRightY), FVector2D(TopLeftX, TopLeftY), 2.f, iColor);
				ZeroGUI::Draw_Line(FVector2D(TopLeftX, TopLeftY), FVector2D(BottomLeftX, BottomLeftY), 2.f, iColor);
			}
			else
			{
				ZeroGUI::Draw_Line(FVector2D(BottomLeftX, BottomLeftY), FVector2D(BottomRightX, BottomRightY), 2.f, FLinearColorPalette::Purple);
				ZeroGUI::Draw_Line(FVector2D(BottomRightX, BottomRightY), FVector2D(TopRightX, TopRightY), 2.f, FLinearColorPalette::Purple);
				ZeroGUI::Draw_Line(FVector2D(TopRightX, TopRightY), FVector2D(TopLeftX, TopLeftY), 2.f, FLinearColorPalette::Purple);
				ZeroGUI::Draw_Line(FVector2D(TopLeftX, TopLeftY), FVector2D(BottomLeftX, BottomLeftY), 2.f, FLinearColorPalette::Purple);
			}
		}

		if (jew::settings.bSAIM)
		{
			UTexture2D* DefaultTexture = Canvas->DefaultTexture;

			float BoxX = TopLeftX;
			float BoxY = TopLeftY;
			float BoxWidth = BottomRightX - TopLeftX;
			float BoxHeight = BottomRightY - TopLeftY;

			FLinearColor FillColor = IsVisible ? FLinearColor(1.0f, 1.0f, 1.0f, 0.3f) : FLinearColor(0.5f, 0.0f, 0.5f, 0.3f);

			for (float y = 0.0f; y < BoxHeight; y++)
			{
				Canvas->K2_DrawTexture(
					DefaultTexture,
					FVector2D(BoxX, BoxY + y),
					FVector2D(BoxWidth, 1.0f),
					FVector2D(),
					FVector2D(1.0f, 1.0f),
					FillColor,
					EBlendMode::BLEND_Translucent,
					0.0f,
					FVector2D()
				);
			}
		}

		if (jew::settings.bFa)
		{
			if (Player != AcknowledgedPawn)
			{
				FVector2D center = UE->ScreenCenter;

				float dx = vHeadScreen.X - center.X;
				float dy = vHeadScreen.Y - center.Y;

				float angle = std::atan2(dy, dx);
				float arrowDistance = jew::settings.fAimFOV + 20.0f;
				float arrowSize = 10.0f;

				FVector2D ArrowPosition = FVector2D(
					center.X + std::cos(angle) * arrowDistance,
					center.Y + std::sin(angle) * arrowDistance
				);

				FVector2D tip = FVector2D(
					ArrowPosition.X + std::cos(angle) * arrowSize,
					ArrowPosition.Y + std::sin(angle) * arrowSize
				);

				FVector2D left = FVector2D(
					ArrowPosition.X + std::cos(angle - 2.5f) * arrowSize * 0.8f,
					ArrowPosition.Y + std::sin(angle - 2.5f) * arrowSize * 0.8f
				);

				FVector2D right = FVector2D(
					ArrowPosition.X + std::cos(angle + 2.5f) * arrowSize * 0.8f,
					ArrowPosition.Y + std::sin(angle + 2.5f) * arrowSize * 0.8f
				);

				FLinearColor ArrowColor = IsVisible
					? FLinearColor(1, 1, 1, 1)
					: FLinearColor(0.5f, 0.f, 0.5f, 1);

				ZeroGUI::Draw_Line(tip, left, 1.0f, ArrowColor);
				ZeroGUI::Draw_Line(left, right, 1.0f, ArrowColor);
				ZeroGUI::Draw_Line(right, tip, 1.0f, ArrowColor);
			}
		}



		if (jew::settings.bSkipKnocked && Player->IsDBNO()) continue;

		auto AimbotBoneScreen = Utils::Project(HeadBone, Canvas);

		if (Utils::InCircle(jew::settings.fAimFOV, AimbotBoneScreen))
		{
			float DistanceToCenter = Utils::Vector_Distance2D(UE->ScreenCenter, AimbotBoneScreen);

			if (DistanceToCenter < ClosestDistanceToCenter)
			{
				ClosestDistanceToCenter = DistanceToCenter;
				ClosestPlayer = Player;
			}
		}
	}

		if (jew::settings.bSkid)
		{
			float radius = jew::settings.fAimFOV;
			int segments = 360;
			float degToRad = 0.0174533f;

			FVector2D center = UE->ScreenCenter;

			for (int i = 0; i < segments; ++i)
			{
				float angle1 = i * degToRad;
				float angle2 = (i + 1) * degToRad;

				FVector2D point1 = FVector2D(center.X + cosf(angle1) * radius, center.Y + sinf(angle1) * radius);
				FVector2D point2 = FVector2D(center.X + cosf(angle2) * radius, center.Y + sinf(angle2) * radius);

				ZeroGUI::Draw_Line(point1, point2, 1.6f, FLinearColor(0.5f, 0.0f, 0.5f, 1.0f));
			}
		}

		if (jew::settings.bNoSpread)
		{
			if (AcknowledgedPawn)
			{
				auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon;

				if (!CurrentWeapon->IsA(AFortWeaponRanged::StaticClass()))
					return;

				SDK::FFortRangedWeaponStats* RangedWeaponStats = (SDK::FFortRangedWeaponStats*)UE->CurrentWeaponStats;

				if (RangedWeaponStats)
				{
					RangedWeaponStats->Spread = 0.0f;
					RangedWeaponStats->SpreadDownsights = 0.0f;
					RangedWeaponStats->StandingStillSpreadMultiplier = 0.0f;
					RangedWeaponStats->AthenaJumpingFallingSpreadMultiplier = 0.0f;
					RangedWeaponStats->AthenaSprintingSpreadMultiplier = 0.0f;
					RangedWeaponStats->MinSpeedForSpreadMultiplier = FLT_MAX;
					RangedWeaponStats->MaxSpeedForSpreadMultiplier = FLT_MAX;
				}
			}
		}


		if (jew::settings.bNoRecoil)
		{
			if (AcknowledgedPawn)
			{
				auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon;

				if (!CurrentWeapon->IsA(AFortWeaponRanged::StaticClass()))
					return;

				SDK::FFortRangedWeaponStats* RangedWeaponStats = (SDK::FFortRangedWeaponStats*)UE->CurrentWeaponStats;

				if (RangedWeaponStats)
				{
					RangedWeaponStats->RecoilDownsightsMultiplier = 0.0f;
					RangedWeaponStats->RecoilHoriz = 0.0f;
					RangedWeaponStats->RecoilHorizScale = 0.0f;
					RangedWeaponStats->RecoilInterpSpeed = 0.0f;
					RangedWeaponStats->RecoilRecoveryDelay = 0.0f;
				}
			}
		}


		if (jew::settings.bNoReload)
		{
			if (AcknowledgedPawn)
			{
				auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon;

				if (!CurrentWeapon->IsA(AFortWeaponRanged::StaticClass()))
					return;

				SDK::FFortRangedWeaponStats* RangedWeaponStats = (SDK::FFortRangedWeaponStats*)UE->CurrentWeaponStats;

				if (RangedWeaponStats)
				{
					RangedWeaponStats->ReloadTime = 0.0f;
				}
			}
		}

		if (jew::settings.bEditEnemyBuilds)
		{
			if (AcknowledgedPawn)
			{
				auto PlayerController = reinterpret_cast<AFortPlayerController*>(UE->World->OwningGameInstance->LocalPlayers[0]->PlayerController);

				auto PlayerState = reinterpret_cast<SDK::AFortPlayerStateAthena*>(AcknowledgedPawn->PlayerState);

				SDK::ABuildingActor* TargetedBuilding = PlayerController->TargetedBuilding;

				if (TargetedBuilding)
				{
					TargetedBuilding->TeamIndex = PlayerState->TeamIndex;
				}
			}
		}

		/*bool bHasResetEquipAnimation = false;

		if (jew::settings.bNoEquipAnimation && !bHasResetEquipAnimation)
		{
			AcknowledgedPawn->CurrentWeapon->bDisableEquipAnimation = true;
		}
		else if (!jew::settings.bNoEquipAnimation && !bHasResetEquipAnimation)
		{
			AcknowledgedPawn->CurrentWeapon->bDisableEquipAnimation = false;
			bHasResetEquipAnimation = true;
		}*/

		if (jew::settings.bAutoWeapons)
		{
			AcknowledgedPawn->CurrentWeapon->WeaponData->TriggerType = EFortWeaponTriggerType::Automatic;
		}

		//bool bHasResetADSState = false;

		//if (jew::settings.bReloadWhileAiming && !bHasResetADSState)
		//{
		//	AcknowledgedPawn->bADSWhileNotOnGround = true;
		//}
		//else if (!jew::settings.bReloadWhileAiming && !bHasResetADSState)
		//{
		//	AcknowledgedPawn->bADSWhileNotOnGround = false;
		//	bHasResetADSState = true;
		//}

		float DefaultFOV = -1.0f;

		if (jew::settings.bFOVChanger)
		{
			if (DefaultFOV < 0.0f)
			{
				DefaultFOV = PlayerController->PlayerCameraManager->GetFOVAngle();
			}

			PlayerController->FOV(jew::settings.fFOVChangerValue);
		}
		else
		{
			if (DefaultFOV >= 0.0f)
			{
				PlayerController->FOV(DefaultFOV);
				DefaultFOV = -1.0f;
			}
		}

		if (ClosestPlayer && jew::settings.bBulletTeleport)
		{
			SDK::TArray<SDK::AActor*> Bullets;
			SDK::UGameplayStatics::GetAllActorsOfClass(UE->World, SDK::AFortProjectileBase::StaticClass(), &Bullets);
			for (int i = 0; i < Bullets.Num(); i++)
			{
				if (!Bullets.IsValidIndex(i)) continue;
				auto Projectile = (SDK::AFortProjectileBase*)Bullets[i];
				if (Projectile->GetGameTimeSinceCreation() > 7.5f) continue;

				SDK::FHitResult fhit;
				Projectile->K2_SetActorLocationAndRotation(ClosestPlayer->Mesh->GetSocketLocation(UE->KismetStringLibrary->Conv_StringToName(L"head")), SDK::FRotator(0, 0, 0), false, &fhit, false);
			}
		}

		if (jew::settings.bAimbot && ClosestPlayer)
		{
			if (auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon)
			{
				if (!CurrentWeapon->IsA(AFortWeaponRanged::StaticClass()))
					return;

				auto ClosestMesh = ClosestPlayer->Mesh;

				auto IsVisible = ClosestPlayer->WasRecentlyRendered(0.f);

				if (!IsVisible)
					return;

				auto ChestBone = ClosestMesh->GetSocketLocation(UE->KismetStringLibrary->Conv_StringToName(L"head"));

				float Distance = Utils::Vector_Distance(CameraLocation, ChestBone);

				auto WorldSecondsDelta = UE->GameplayStatics->GetWorldDeltaSeconds(GWorld);

				if (CurrentWeapon->IsProjectileWeapon() && jew::settings.bPrediction)
				{
					ChestBone = Utils::Predict(CameraLocation, ChestBone, ClosestPlayer->GetVelocity(), CurrentWeapon);
				}

				auto ChestRotation = UE->KismetMathLibrary->FindLookAtRotation(CameraLocation, ChestBone);

				ChestRotation = Utils::SmoothMe(CameraRotation, ChestRotation, jew::settings.fSmooth);

				if (GetAsyncKeyState(jew::settings.iAimbotKey))
				{
					UE->PlayerController->SetControlRotation(ChestRotation);
				}
			}

			if (jew::settings.bPenisaim && ClosestPlayer)
			{
				if (auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon)
				{
					if (!CurrentWeapon->IsA(AFortWeaponRanged::StaticClass()))
						return;

					auto ClosestMesh = ClosestPlayer->Mesh;

					auto IsVisible = ClosestPlayer->WasRecentlyRendered(0.f);

					if (!IsVisible)
						return;

					auto ChestBone = ClosestMesh->GetSocketLocation(UE->KismetStringLibrary->Conv_StringToName(L"pelvis"));

					float Distance = Utils::Vector_Distance(CameraLocation, ChestBone);

					auto WorldSecondsDelta = UE->GameplayStatics->GetWorldDeltaSeconds(GWorld);

					if (CurrentWeapon->IsProjectileWeapon() && jew::settings.bPrediction)
					{
						ChestBone = Utils::Predict(CameraLocation, ChestBone, ClosestPlayer->GetVelocity(), CurrentWeapon);
					}

					auto ChestRotation = UE->KismetMathLibrary->FindLookAtRotation(CameraLocation, ChestBone);

					ChestRotation = Utils::SmoothMe(CameraRotation, ChestRotation, jew::settings.fSmooth);

					if (GetAsyncKeyState(jew::settings.iAimbotKey))
					{
						UE->PlayerController->SetControlRotation(ChestRotation);
					}
				}
			}
		}

		if (jew::settings.bDemoSpeed)
		{
			if (UE->World)
			{
				SDK::AWorldSettings* WorldSettings = UE->TimeDilation;

				if (WorldSettings)
				{
					WorldSettings->TimeDilation = jew::settings.DemoSpeedValue;
				}
			}
		}
	}

void SetupClasses()
{
	UE->Font = UObject::FindObject<UFont>("Font BurbankSmall.BurbankSmall");

	UE->DrawingFont = UObject::FindObject<UFont>("Font BurbankSmall.BurbankSmall");

	UE->DrawingFont->LegacyFontSize = 10;

	UE->KismetMathLibrary = (SDK::UKismetMathLibrary*)SDK::UKismetMathLibrary::StaticClass();
	UE->KismetSystemLibrary = (SDK::UKismetSystemLibrary*)SDK::UKismetSystemLibrary::StaticClass();
	UE->KismetStringLibrary = (SDK::UKismetStringLibrary*)SDK::UKismetStringLibrary::StaticClass();
	UE->FortKismetLibrary = (SDK::UFortKismetLibrary*)SDK::UFortKismetLibrary::StaticClass();

	UE->GameplayStatics = (SDK::UGameplayStatics*)SDK::UGameplayStatics::StaticClass();

	if (UE->World->OwningGameInstance && UE->World->OwningGameInstance->LocalPlayers.IsValidIndex(0)) UE->LocalPlayer = UE->World->OwningGameInstance->LocalPlayers[0];
	UE->PlayerController = UE->LocalPlayer->PlayerController;

	UE->LeftMouseButton = SDK::FKey{ SDK::FName{UE->KismetStringLibrary->Conv_StringToName(L"LeftMouseButton") } };
	UE->RightMouseButton = SDK::FKey{ SDK::FName{UE->KismetStringLibrary->Conv_StringToName(L"RightMouseButton") } };
	UE->Insert = SDK::FKey{ SDK::FName{UE->KismetStringLibrary->Conv_StringToName(L"Insert") } };
	UE->LeftControl = SDK::FKey{ SDK::FName{UE->KismetStringLibrary->Conv_StringToName(L"Left Control") } };
	UE->SpaceBar = SDK::FKey{ SDK::FName{UE->KismetStringLibrary->Conv_StringToName(L"SpaceBar") } };
	UE->W = SDK::FKey{ SDK::FName{UE->KismetStringLibrary->Conv_StringToName(L"W") } };
	UE->A = SDK::FKey{ SDK::FName{UE->KismetStringLibrary->Conv_StringToName(L"A") } };
	UE->S = SDK::FKey{ SDK::FName{UE->KismetStringLibrary->Conv_StringToName(L"S") } };
	UE->D = SDK::FKey{ SDK::FName{UE->KismetStringLibrary->Conv_StringToName(L"D") } };
	UE->LeftShift = SDK::FKey{ SDK::FName{UE->KismetStringLibrary->Conv_StringToName(L"Left Shift") } };
}

void MainRender(SDK::UGameViewportClient* Viewport, SDK::UCanvas* Canvas)
{
	PostRender(Viewport, Canvas);

	return DrawTransitionOriginal(Viewport, Canvas);
}

void Main()
{
	UE->Engine = SDK::UEngine::GetEngine();
	UE->World = SDK::UWorld::GetWorld();

	auto GameInstance = UE->World->OwningGameInstance;
	auto LocalPlayers = GameInstance->LocalPlayers;
	auto ViewportClient = LocalPlayers[0]->ViewportClient;

	SetupClasses();

	if (NewHook::ViewportHook.Initialize(ViewportClient))
	{
		NewHook::ViewportHook.Insert(&MainRender, Offset::PostRender, &DrawTransitionOriginal);

		NewHook::ViewportHook.SwapContext();
	}

}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH && ValidateLaunch())
	{
		DisableThreadLibraryCalls(module);

		std::thread([module]()
			{
				/*HWND hWnd = NULL;

				while ((hWnd = FindWindowW(NULL, L"Goonme")) == NULL)
				{
					Sleep(500);
				}*/

				int Result = MessageBoxW(NULL, L"Press OK to Inject.\nIN GAME", L"Injector", MB_OK | MB_ICONQUESTION);

				if (Result == IDOK)
				{
					AllocConsole();
					FILE* fileptr;
					freopen_s(&fileptr, "CONOUT$", "w", stdout);
					freopen_s(&fileptr, "CONOUT$", "w", stderr);
					freopen_s(&fileptr, "CONIN$", "r", stdin);

					Main();

					Sleep(INFINITE);
				}

			}).detach();
	}

	return TRUE;
}



extern "C" __declspec(dllexport) int Fellative(int code, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(NULL, code, wParam, lParam);
}
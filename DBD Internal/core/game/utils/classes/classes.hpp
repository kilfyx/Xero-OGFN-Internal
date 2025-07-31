#pragma once
#include <SDK.hpp>
#include <Windows.h>
#include <corecrt_math_defines.h>

class CORE_C
{
public:
	static CORE_C* Instance()
	{
		static CORE_C instance;
		return &instance;
	}
};

class UnrealEngine
{
public:
	static inline PBYTE GObjects = NULL;
	static inline SDK::UWorld* World = NULL;
	static inline SDK::UEngine* Engine = NULL;
	static inline SDK::FMinimalViewInfo* Camera;
	static inline SDK::APlayerController* PlayerController = NULL;
	static inline SDK::AWorldSettings* TimeDilation = NULL;
	static inline SDK::UGameInstance* OwningGameInstance = NULL;
	static inline SDK::FFortBaseWeaponStats* CurrentWeaponStats = NULL;
	static inline SDK::ULocalPlayer* LocalPlayer = NULL;
	static inline SDK::UKismetMathLibrary* KismetMathLibrary = NULL;
	static inline SDK::AFortWeapon* CurrentWeapon = NULL;
	static inline SDK::UKismetStringLibrary* KismetStringLibrary = NULL;
	static inline SDK::UFortKismetLibrary* FortKismetLibrary = NULL;
	static inline SDK::UKismetSystemLibrary* KismetSystemLibrary = NULL;
	static inline SDK::UGameplayStatics* GameplayStatics = NULL;
	static inline SDK::UFont* Font = NULL;
	static inline SDK::UFont* DrawingFont = NULL;
	static inline SDK::UCanvas* CachedCanvas = NULL;
	static inline SDK::FVector2D ScreenCenter = SDK::FVector2D();
	static inline SDK::FVector2D ScreenSize = SDK::FVector2D();
	static inline SDK::FKey LeftMouseButton = SDK::FKey();
	static inline SDK::FKey RightMouseButton = SDK::FKey();
	static inline SDK::FKey SpaceBar = SDK::FKey();
	static inline SDK::FKey W = SDK::FKey();
	static inline SDK::FKey A = SDK::FKey();
	static inline SDK::FKey S = SDK::FKey();
	static inline SDK::FKey D = SDK::FKey();
	static inline SDK::FKey LeftShift = SDK::FKey();
	static inline SDK::FKey LeftControl = SDK::FKey();
	static inline SDK::FKey Insert = SDK::FKey();
	static inline bool DrawMenu = bool(true);
	float FontSize;
};

inline UnrealEngine* UE = new UnrealEngine;



namespace Utils
{

	inline SDK::FRotator CalcAngle(const SDK::FVector& src, const SDK::FVector& dst)
	{
		SDK::FVector delta = dst - src;
		float hyp = UE->KismetMathLibrary->Sqrt(delta.X * delta.X + delta.Y * delta.Y);

		SDK::FRotator rot;
		rot.Pitch = -UE->KismetMathLibrary->Atan2(delta.Z, hyp) * (180.0f / M_PI);
		rot.Yaw = UE->KismetMathLibrary->Atan2(delta.Y, delta.X) * (180.0f / M_PI);
		rot.Roll = 0.0f;

		return rot;
	}


	inline SDK::FVector2D Project(SDK::FVector WorldLocation, SDK::UCanvas* Canvas)
	{
		SDK::FVector ScreenLocation = Canvas->K2_Project(WorldLocation);

		if (ScreenLocation.Z > 0.f)
		{
			return SDK::FVector2D(ScreenLocation.X, ScreenLocation.Y);
		}

		return SDK::FVector2D(-1.f, -1.f);
	}

	inline float Vector_Distance(SDK::FVector V1, SDK::FVector V2)
	{
		return V1.GetDistanceTo(V2);
	}


	inline float Vector_Distance2D(SDK::FVector2D V1, SDK::FVector2D V2)
	{
		return V1.GetDistanceTo(V2);
	}

	inline bool InRect(double Radius, SDK::FVector2D ScreenLocation)
	{
		return UE->ScreenCenter.X >= (UE->ScreenCenter.X - Radius) && UE->ScreenCenter.X <= (UE->ScreenCenter.X + Radius) &&
			ScreenLocation.Y >= (ScreenLocation.Y - Radius) && ScreenLocation.Y <= (ScreenLocation.Y + Radius);
	}

	inline bool InCircle(double Radius, SDK::FVector2D ScreenLocation)
	{
		if (InRect(Radius, ScreenLocation))
		{
			double dx = UE->ScreenCenter.X - ScreenLocation.X; dx *= dx;
			double dy = UE->ScreenCenter.Y - ScreenLocation.Y; dy *= dy;
			return dx + dy <= Radius * Radius;
		} return false;
	}

	inline SDK::FLinearColor GetColorByTier(SDK::EFortItemTier Rarity)
	{
		SDK::FLinearColor RenderColor = SDK::FLinearColor(1.0, 1.0, 1.0, 1);

		switch (Rarity) {
		case SDK::EFortItemTier::No_Tier:
			RenderColor = SDK::FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case SDK::EFortItemTier::I:
			RenderColor = SDK::FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case SDK::EFortItemTier::II:
			RenderColor = SDK::FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		case SDK::EFortItemTier::III:
			RenderColor = SDK::FLinearColor(0.0f, 0.80f, 1.60f, 1.0f);
			break;
		case SDK::EFortItemTier::IV:
			RenderColor = SDK::FLinearColor(0.6f, 0.2f, 0.8f, 1.0f);
			break;
		case SDK::EFortItemTier::V:
			RenderColor = SDK::FLinearColor(1.0f, 0.55f, 0.0f, 1.0f);
			break;
		case SDK::EFortItemTier::VI:
			RenderColor = SDK::FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		}

		return RenderColor;
	}

	inline float GetBulletDrop(float height, float DepthPlayerTarget, float BulletVelocity, float GravityAcceleration)
	{
		float Pitch = (UE->KismetMathLibrary->Atan2(height, DepthPlayerTarget));
		float BulletVelocityXY = BulletVelocity * UE->KismetMathLibrary->Cos(Pitch);
		float Time = DepthPlayerTarget / BulletVelocityXY;
		float TotalVerticalDrop = (0.5f * GravityAcceleration * Time * Time);
		return TotalVerticalDrop;
	}

	inline SDK::FVector Predict(SDK::FVector CameraLocation, SDK::FVector CurrentLocation, SDK::FVector ComponentVelocity, SDK::AFortWeapon* CurrentWeapon)
	{
		auto ProjectileSpeed = CurrentWeapon->GetProjectileSpeed(CurrentWeapon->GetChargePercent());
		auto ProjectileGravity = -337;

		SDK::FVector StartLocation = CurrentLocation;
		auto PlayerDistance = Vector_Distance(CameraLocation, StartLocation);

		float Height = StartLocation.Z - CameraLocation.Z;
		float DepthPlayerTarget = UE->KismetMathLibrary->Sqrt(pow(StartLocation.Y - CameraLocation.Y, 2) + pow(StartLocation.X - CameraLocation.X, 2));
		float BulletDrop = GetBulletDrop(Height, DepthPlayerTarget, ProjectileSpeed, ProjectileGravity);
		float pitch = (UE->KismetMathLibrary->Atan2(Height, DepthPlayerTarget));
		float BulletVelocityXY = ProjectileSpeed * UE->KismetMathLibrary->Cos(pitch);
		float TimeToTarget = DepthPlayerTarget / BulletVelocityXY;

		StartLocation.X += (TimeToTarget * ComponentVelocity.X);
		StartLocation.Y += (TimeToTarget * ComponentVelocity.Y);
		StartLocation.Z += (TimeToTarget * ComponentVelocity.Z);
		StartLocation.Z -= BulletDrop;

		return StartLocation;
	}

	inline SDK::FRotator SmoothMe(SDK::FRotator StartRotation, SDK::FRotator EndRotation, float SmoothScale)
	{
		SDK::FRotator ret = EndRotation;

		ret.Pitch -= StartRotation.Pitch;
		ret.Yaw -= StartRotation.Yaw;

		//clamp
		if (ret.Yaw < -180.0f)
			ret.Yaw += 360.0f;

		if (ret.Yaw > 180.0f)
			ret.Yaw -= 360.0f;

		if (ret.Pitch < -74.0f)
			ret.Pitch = -74.0f;

		if (ret.Pitch > 74.0f)
			ret.Pitch = 74.0f;

		ret.Pitch = (ret.Pitch / SmoothScale) + StartRotation.Pitch;
		ret.Yaw = (ret.Yaw / SmoothScale) + StartRotation.Yaw;

		//clamp
		if (ret.Yaw < -180.0f)
			ret.Yaw += 360.0f;

		if (ret.Yaw > 180.0f)
			ret.Yaw -= 360.0f;

		if (ret.Pitch < -74.0f)
			ret.Pitch = -74.0f;

		if (ret.Pitch > 74.0f)
			ret.Pitch = 74.0f;

		ret.Roll = 0;

		return ret;
	}
};

inline CORE_C* CORE()
{
	return CORE_C::Instance();
}
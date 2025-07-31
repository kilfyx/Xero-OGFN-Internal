#pragma once
#include <SDK.hpp>
#include <Windows.h>
#include <core/features/weaponstats/weaponstats.hpp>
#include <core/game/utils/classes/classes.hpp>
#include <utils/settings/settings.hpp>
#include <dependencies/memory/memory.hpp>

bool IsValidPointer(void* Address)
{
	if (!Address)
	{
		return false;
	}
	if (IsBadReadPtr(Address, 8))
	{
		return false;
	}

	return true;
}

SDK::FFortBaseWeaponStats* jew::weaponstats_c::WeaponStats(SDK::AFortWeapon* Weapon)
{
    if (!Weapon || !IsValidPointer(Weapon))
        return nullptr;

    auto vtable = *reinterpret_cast<void***>(Weapon);
    if (!vtable || !vtable[229])
        return nullptr;

    using FnType = SDK::FFortBaseWeaponStats * (__fastcall*)(SDK::AFortWeapon*);
    return reinterpret_cast<FnType>(vtable[229])(Weapon);
}

void jew::weaponstats_c::WeaponTick(SDK::AFortWeapon* Weapon)
{
	if (UE->CurrentWeapon != Weapon || UE->CurrentWeapon == nullptr || UE->CurrentWeaponStats == nullptr)
	{
		UE->CurrentWeapon = Weapon;
		UE->CurrentWeaponStats = jew::weaponstats.WeaponStats(Weapon);
	}
}
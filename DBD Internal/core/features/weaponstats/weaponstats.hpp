#pragma once
#include <SDK.hpp>

namespace jew
{
	class weaponstats_c
	{
	public:

		SDK::FFortBaseWeaponStats* WeaponStats(SDK::AFortWeapon* Weapon);
		auto WeaponTick(SDK::AFortWeapon* Weapon) -> void;
	};

	inline weaponstats_c weaponstats;
}
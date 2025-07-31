#pragma once
#include <iostream>
#include <SDK.hpp>

namespace jew
{
	class menu_c
	{
	public:

		auto DrawMenu(UCanvas* Canvas, bool ShowMenu) -> void;
	};

	inline menu_c menu;
}
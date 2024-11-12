

#pragma once

#include "imgui.h"
#include <string>
#include <utility>

namespace FontManager {

	// each weight are seperated by 1000, so weight and size could be combined for keys in fonts map
	enum FontWeight : uint16_t {
		Regular = 0,
		Medium = 1000,
		SemiBold = 2000,
		Bold = 3000,
		ExtraBold = 4000,
		Black = 5000
	};

	// must be called before using any function from FontManager
	void LoadFonts();

	ImFont* GetFont(FontWeight weight, uint16_t size);
	ImFont* GetFont(const std::string& fontName);

}

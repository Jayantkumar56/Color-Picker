

#include "FontManager.h"

#include <iostream>
#include <unordered_map>

namespace FontManager {

	static std::unordered_map<uint16_t, ImFont*>    Fonts;
	static std::unordered_map<std::string, ImFont*> FontsWithName;

	static std::string_view GetFontPathWithWeight(FontWeight weight) {
		switch (weight) {
			case FontWeight::Regular:	return "assets/Fonts/Schibsted_Grotesk/static/SchibstedGrotesk-Regular.ttf";
			case FontWeight::Medium:	return "assets/Fonts/Schibsted_Grotesk/static/SchibstedGrotesk-Medium.ttf";
			case FontWeight::SemiBold:	return "assets/Fonts/Schibsted_Grotesk/static/SchibstedGrotesk-SemiBold.ttf";
			case FontWeight::Bold:		return "assets/Fonts/Schibsted_Grotesk/static/SchibstedGrotesk-Bold.ttf";
			case FontWeight::ExtraBold:	return "assets/Fonts/Schibsted_Grotesk/static/SchibstedGrotesk-ExtraBold.ttf";
			case FontWeight::Black:		return "assets/Fonts/Schibsted_Grotesk/static/SchibstedGrotesk-Black.ttf";
		}

		std::cout << "Invalid Font Weight specified!";
		return "";
	}

	static inline void LoadFont(ImGuiIO& io, FontWeight weight, uint16_t size, const char* name = nullptr) {
		Fonts[weight + size] = io.Fonts->AddFontFromFileTTF(GetFontPathWithWeight(weight).data(), static_cast<float>(size));

		if (name != nullptr) {
			FontsWithName[name] = Fonts[weight + size];
		}
	}

	void LoadFonts() {
		ImGuiIO& io = ImGui::GetIO();

		LoadFont(io, FontWeight::SemiBold,  22);
		LoadFont(io, FontWeight::Regular,   21);
	}

	ImFont* GetFont(FontWeight weight, uint16_t size) {
		if (!Fonts.contains(weight + size)) {
			ImGuiIO& io = ImGui::GetIO();
			Fonts[weight + size] = io.Fonts->AddFontFromFileTTF(GetFontPathWithWeight(weight).data(), static_cast<float>(size));
		}

		return Fonts[weight + size];
	}

	ImFont* GetFont(const std::string& fontName) {
		if (!FontsWithName.contains(fontName)) {
			std::cout << "Required font with name %s do not exist" << fontName;
		}

		return FontsWithName[fontName];
	}

}

#include "UIFontManager.h"

#include <unordered_map>

static std::unordered_map<std::string, ImFont *> fonts;
static float s_UIScale = 1.0f;

void LoadUIFont(std::string name, float pizelSize, std::string path)
{
	ImGuiIO &io = ImGui::GetIO();
	fonts[name] = io.Fonts->AddFontFromFileTTF(path.c_str(), pizelSize * s_UIScale);
}

void SetUIFontScale(float scale)
{
	if (scale > 0.0f)
	{
		s_UIScale = scale;
	}
}

ImFont *GetUIFont(std::string name)
{
	if (fonts.find(name) != fonts.end())
	{
		return fonts[name];
	}

	return nullptr;
}

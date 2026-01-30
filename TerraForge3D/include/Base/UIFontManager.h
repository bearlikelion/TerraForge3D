#pragma once

#include "imgui.h"
#include <string>

void LoadUIFont(std::string name, float pixelSize, std::string path);
void SetUIFontScale(float scale);
ImFont *GetUIFont(std::string name);

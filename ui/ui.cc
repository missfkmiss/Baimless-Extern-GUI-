#pragma once
#include "ui.hh"
#include "../globals.hh"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"


void ui::render() 
{
	ImGui::Checkbox("ESP Enabled", &globals.actives);
	ImGui::Checkbox("BunnyHop", &globals.bhop);
	ImGui::Checkbox("Glow", &globals.glow);
	ImGui::Checkbox("Aimbot", &globals.activeAimbot);

}

void ui::init(LPDIRECT3DDEVICE9 device) {
    dev = device;
	
    // colors
    ImGui::StyleColorsDark();

	if (window_pos.x == 0) {
		RECT screen_rect{};
		GetWindowRect(GetDesktopWindow(), &screen_rect);
		screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
		window_pos = (screen_res - window_size) * 0.5f;

		// init images here
	}
}
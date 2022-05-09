#pragma once
#include <unordered_map>

class c_globals {
public:
	bool active = true;
	char user_name[255] = "user";
	char pass_word[255] = "pass";
	bool actives = false;
	bool bhop = false;
	bool glow = false;
	bool activeAimbot = false;
	bool activemodules = false;
	bool trigger = false;
	HANDLE hProcess;
	uintptr_t modulebase = 0;
	uintptr_t engineAddress = 0;
	float smooth = 20.f;
	HDC hdc;
	int screenX;
	int screenY;
};

inline c_globals globals;
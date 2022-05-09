#pragma once
#include "Main.h"
#include "ui/ui.hh"
#include "globals.hh"
// Main code
#include "math.h"
#include "draw.h"
#include "Mem.h"

bool csgo = false;
bool hookmodules;

template <class dataType>
void WPM(dataType valToWrite, DWORD addressToWrite)
{
	WriteProcessMemory(globals.hProcess, (PVOID)addressToWrite, &valToWrite, sizeof(dataType), 0);
}

template<typename T> T RPM(SIZE_T address) {
	//Aqui setamos a variavel que vai receber o valor da leitura para o return statement
	T buffer;

	//Lemos o endereço na memória (address) e passamos o valor pra dentro da nossa variável (buffer)
	ReadProcessMemory(globals.hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);

	//E retornamos a variavel que contém o valor da leitura
	return buffer;
}

Vector3	getEntBonePos(DWORD playerBase, int boneID)
{
	Matrix3x4_T bone = RPM<Matrix3x4_T>(playerBase + offsets::m_dwBoneMatrix + boneID * 0x30);

	return{
		bone.matrix[0][3],
		bone.matrix[1][3],
		bone.matrix[2][3]
	};
}
void esp()
{
	Vector3 posLocal;
	while (globals.actives)
	{

		view_matrix_t vm = RPM<view_matrix_t>(globals.modulebase + offsets::dwViewMatrix);

		uintptr_t localPlayerPtr = RPM<DWORD>(globals.modulebase + offsets::dwLocalPlayer);

		int localTeam = RPM<int>((globals.modulebase + offsets::dwLocalPlayer) + offsets::m_iTeamNum);

		for (int i = 1; i < 32; ++i)
		{

			uintptr_t pEnt = RPM<DWORD>(globals.modulebase + offsets::dwEntityList + (i * 0x10));

			int enemyTeam = RPM<int>(pEnt + offsets::m_iTeamNum);

			int health = RPM<int>(pEnt + offsets::m_iHealth);


			Vector3 pos = RPM<Vector3>(pEnt + offsets::m_vecOrigin);
			Vector3 posLocal = RPM<Vector3>(localPlayerPtr + offsets::m_vecOrigin);

			Vector3 posHead;
			posHead.x = pos.x;
			posHead.y = pos.y;
			posHead.z = pos.z + 75.f;

			Vector3 bonePos = getEntBonePos(pEnt, 8);


			Vector3 screenPos = WorldToScreen(pos, vm);
			Vector3 screenHeadPos = WorldToScreen(posHead, vm);
			Vector3 screenboneheadPos = WorldToScreen(bonePos, vm);


			float height = screenHeadPos.y - screenPos.y;

			float width = height / 2.4f;


			float healthheight = health * height / 100;


			float healthwidth = height / 20.f;


			if (screenPos.z >= 0.01f && enemyTeam != localTeam && health > 0 && health < 101)
			{

				DrawBackGroundHealth(screenPos.x + (width / 1.8), screenPos.y, healthwidth, height);
				DrawHealth(screenPos.x + (width / 1.8), screenPos.y, healthwidth, healthheight);
			}
		}
	}
}

void triggerbot()
{
	while (globals.trigger)
	{
		uintptr_t localPlayer = RPM<uintptr_t>(globals.modulebase + offsets::dwLocalPlayer);
		int entity_id = RPM<int>(localPlayer + offsets::m_iCrosshairId);
		int entity = RPM<int>(globals.modulebase + offsets::dwEntityList + entity_id * 0x10);
		int entityTeam = RPM<int>(entity + offsets::m_iTeamNum);
		int localplayerTeam = RPM<int>(localPlayer + offsets::m_iTeamNum);

		if (GetAsyncKeyState(VK_LEFT))
		{
			if (entity_id > 0 && entity_id < 32 && localplayerTeam != entityTeam)
			{
				WPM<int>(6, globals.modulebase + offsets::dwForceAttack);
			}
		}
	}
}

void bunnyhop()
{
	while (globals.bhop)
	{
		uintptr_t localPlayer = RPM<uintptr_t>(globals.modulebase + offsets::dwLocalPlayer);
		int flags = RPM<int>(localPlayer + offsets::m_fFlags);
		if (GetAsyncKeyState(VK_SPACE) & 0x8000 && (flags == 257))
		{
			WPM<int>(5, globals.modulebase + offsets::dwForceJump);
		}
		else if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !(flags == 257))
		{
			WPM<int>(4, globals.modulebase + offsets::dwForceJump);
			WPM<int>(5, globals.modulebase + offsets::dwForceJump);
			WPM<int>(4, globals.modulebase + offsets::dwForceJump);
		}
		else
		{
			WPM<int>(4, globals.modulebase + offsets::dwForceJump);
		}
	}

}

void glow()
{
	while (globals.glow)
	{
		uintptr_t localPlayer = RPM<uintptr_t>(globals.modulebase + offsets::dwLocalPlayer);
		uintptr_t glowObjectManager = RPM<uintptr_t>(globals.modulebase + offsets::dwGlowObjectManager);
		uintptr_t localPlayerTeam = RPM<uintptr_t>(localPlayer + offsets::m_iTeamNum);

		for (auto i = 1; i <= 32; ++i)
		{
			auto entity = RPM<uintptr_t>(globals.modulebase + offsets::dwEntityList + i * 0x10);

			if (!entity)
				continue;

			// dont glow if they are on our team
			if (RPM<uintptr_t>(entity + offsets::m_iTeamNum) == localPlayerTeam)
				continue;

			auto glowIndex = RPM<int32_t>(entity + offsets::m_iGlowIndex);
			auto R = glowObjectManager + (glowIndex * 0x38) + 0x8;
			auto G = glowObjectManager + (glowIndex * 0x38) + 0xC;
			auto B = glowObjectManager + (glowIndex * 0x38) + 0x10;
			auto A = glowObjectManager + (glowIndex * 0x38) + 0x14;
			auto B1 = glowObjectManager + (glowIndex * 0x38) + 0x28;
			auto B2 = glowObjectManager + (glowIndex * 0x38) + 0x29;
			WPM<float>(1.f, R);
			WPM<float>(0.f, G);
			WPM<float>(0.f, B);
			WPM<float>(1.f, A);
			WPM<bool>(true, B1);
			WPM<bool>(false, B2);

		}
	}
}

DWORD playerfunction()
{

	DWORD nextaimplayer = NULL;
	Vector3 poslocal;
	Vector3 screenheadpos;
	float mindistace = 9999;
	view_matrix_t vm = RPM<view_matrix_t>(globals.modulebase + offsets::dwViewMatrix);
	uintptr_t localplayerptr = RPM<DWORD>(globals.modulebase + offsets::dwLocalPlayer);
	for (int i = 1; i < 10; ++i)
	{
		auto pEnt = RPM<DWORD>(globals.modulebase + offsets::dwEntityList + (i * 0x10));
		int health = RPM<int>(pEnt + offsets::m_iHealth);
		Vector3 pos = RPM<Vector3>(pEnt + offsets::m_vecOrigin);
		poslocal = RPM<Vector3>(localplayerptr + offsets::m_vecOrigin);

		Vector3 bonepos = getEntBonePos(pEnt, 8);
		Vector3 screnboneheadpos = WorldToScreen(bonepos, vm);

		Vector3 screenpos = WorldToScreen(pos, vm);
		auto dormant = RPM<bool>(pEnt + offsets::m_bDormant);
		if (health > 0 && health <= 100 && !dormant)
		{
			double calcDist = sqrt(pow((GetSystemMetrics(SM_CXSCREEN) / 2) - screnboneheadpos.x, 2) + pow((GetSystemMetrics(SM_CYSCREEN) / 2) - screnboneheadpos.y, 2));

			if (calcDist < mindistace)
			{
				mindistace = calcDist;
				nextaimplayer = pEnt;
			}
		}
	}
	return nextaimplayer;
}

void aimbot()
{
	while (globals.activeAimbot)
	{
		DWORD target = playerfunction();
		if (target != NULL)
		{
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				uintptr_t clientstate = RPM<DWORD>(globals.engineAddress + offsets::dwClientState);
				view_matrix_t vm = RPM<view_matrix_t>(globals.modulebase + offsets::dwViewMatrix);
				uintptr_t localplayerptr = RPM<DWORD>(globals.modulebase + offsets::dwLocalPlayer);
				Vector3 poslocal = RPM<Vector3>(localplayerptr + offsets::m_vecOrigin);
				Vector3 semitarget = RPM<Vector3>(target + offsets::m_vecOrigin);
				Vector3 aimangles = CalcAngle(poslocal, semitarget);
				WPM<Vector3>(aimangles, clientstate + offsets::dwClientState_ViewAngles);
			}
		}
	}
}





int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	

    // Create application window
 // Create application window
	std::string windowtitle = std::string(ui::window_title);
	std::wstring stemp = std::wstring(windowtitle.begin(), windowtitle.end());
	LPCWSTR sw = stemp.c_str();
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, sw, NULL };
	RegisterClassEx(&wc);
	main_hwnd = CreateWindow(wc.lpszClassName, sw, WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);

    globals.screenX = GetSystemMetrics(SM_CXSCREEN);
    globals.screenY = GetSystemMetrics(SM_CYSCREEN);

    // Setup Dear ImGui context
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; //crutial for not leaving the imgui.ini file
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }


        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            static int init = false;
            if (!init) {
                ui::init(g_pd3dDevice);
                init = true;
            }
            else 
            {
                if (globals.active)
                {
                    ImGui::SetNextWindowPos(ImVec2(ui::window_pos.x, ui::window_pos.y), ImGuiCond_Once);
                    ImGui::SetNextWindowSize(ImVec2(ui::window_size.x, ui::window_size.y));
                    ImGui::SetNextWindowBgAlpha(1.0f);

                    ImGui::Begin(ui::window_title, &globals.active, ui::window_flags);
                    {
                        if (!csgo)
                        {
                            ImGui::Text("Waiting CS:GO");
                            if (GetProcId(L"csgo.exe") != 0)
                            {
                                Sleep(5000);

                                csgo = true;
                            }
                        }
                        else
                        {
							if (ImGui::Button("Hook Modules"))
							{
								
								globals.modulebase = GetModuleBaseAddress(GetProcId(L"csgo.exe"), L"client.dll");
								globals.engineAddress = GetModuleBaseAddress(GetProcId(L"csgo.exe"), L"engine.dll");
								globals.hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetProcId(L"csgo.exe"));
								globals.hdc = GetDC(FindWindowA(NULL, "Counter-Strike: Global Offensive - Direct3D 9"));
								globals.screenX = GetSystemMetrics(SM_CXSCREEN);
								globals.screenY = GetSystemMetrics(SM_CYSCREEN);
								
							}
							if (ImGui::Button("ESP"))
							{
								globals.actives = !globals.actives;
								CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)esp, NULL, 0, NULL));
							}
							if (ImGui::Button("Glow")) 
							{
								globals.glow = !globals.glow;
								CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)glow, NULL, 0, NULL));

							}
							if (ImGui::Button("Bunnyhop"))
							{
								globals.bhop = !globals.bhop;
								CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)bunnyhop, NULL, 0, NULL));

							}
							if (ImGui::Button("Triggerbot"))
							{
								globals.trigger = !globals.trigger;
								CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)triggerbot, NULL, 0, NULL));

							}
							if (ImGui::Button("Aimbot"))
							{
								
								globals.activeAimbot = !globals.activeAimbot;
								CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)aimbot, NULL, 0, NULL));
							}
                        }
                    }
                    ImGui::End();
                }
            }
        }
        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!globals.active) {
            msg.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    CloseHandle(globals.hProcess);
    return 0;

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
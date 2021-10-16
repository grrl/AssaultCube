#include <Windows.h>
#include <imgui.h>
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "d3d9.h"
#include "draw.h"

bool aim;

void SendKey() {
	INPUT input;
	WORD vkey = VK_LEFT; // see link below
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));
}

void SendKeySpace() {
	INPUT input;
	WORD vkey = VK_SPACE; // see link below
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));
}

void ChangeClickability(bool canclick, HWND ownd)
{
	long style = GetWindowLong(
		ownd, GWL_EXSTYLE);
	if (canclick) {
		style &= ~WS_EX_LAYERED;
		RECT correct_window_rect;
		GetWindowRect(hWnd, &correct_window_rect);
		HWND my_new_hwnd;
		float my_width = (correct_window_rect.right - correct_window_rect.left) - 7;
		float my_heigth = (correct_window_rect.bottom - correct_window_rect.top) + 25;
		//SetWindowPos(hWnd, hWnd, correct_window_rect.left - 7, correct_window_rect.top + 25, my_heigth, my_width, SWP_SHOWWINDOW);
		//SetWindowPos(ownd, HWND_TOPMOST, correct_window_rect.left - 7, correct_window_rect.top + 25, my_heigth, my_width, SWP_NOSIZE | SWP_NOMOVE);
		//SetWindowPos(ownd, HWND_NOTOPMOST, correct_window_rect.left - 7, correct_window_rect.top + 25, my_heigth, my_width, SWP_NOSIZE | SWP_NOMOVE);
		SetWindowLong(ownd,
			GWL_EXSTYLE, style);

		SetWindowPos(ownd, ownd, correct_window_rect.left, correct_window_rect.top, my_heigth, my_width, SWP_SHOWWINDOW);

		SetForegroundWindow(ownd);

		windowstate = 1;
	}
	else {
		style |= WS_EX_LAYERED;
		SetWindowLong(ownd,
			GWL_EXSTYLE, style);
		windowstate = 0;
	}
}

void menu() {

	if (windowstate != 1) //You might need to remove this check and call it every time to get keyboard input working. Mouse input works anyway.
	{
		RECT correct_window_rect;
		GetWindowRect(hWnd, &correct_window_rect);

		float my_width = correct_window_rect.right - correct_window_rect.left;
		float my_heigth = correct_window_rect.bottom - correct_window_rect.top;
		//SetWindowPos(hWnd, hWnd, correct_window_rect.left - 20, correct_window_rect.top - 50, my_heigth, my_width, SWP_SHOWWINDOW);

		ChangeClickability(true, hWnd);
		SendKey();
	}


	// convert now to tm struct for UTC
	//tm *gmtm = gmtime(&now);
	//dt = asctime(gmtm);
	char stamp[256] = "Foreground 10 10  ";
	DrawString(stamp, 10, 10, 255, 0, 255, dx_FontCalibri); // Put Main procedure here like ESP etc.

	
	ImGui::SetNextWindowSize(ImVec2(200, 200));
	//ImGui::SetNextWindowPos(ImVec2(200, 200));
	/*
	if (ImGui::Begin("dbd", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		/*ImGuiWindowFlags_AlwaysAutoResize |*/
		//ImGuiWindowFlags_NoSavedSettings
	//))
	ImGui::Begin("Assault Cube");
	{
		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Aimb##aim", &aim);
		ImGui::Checkbox("Aimb##aim", &aim);
		ImGui::Checkbox("Aimb##aim", &aim);

		ImGui::End();
	}
}
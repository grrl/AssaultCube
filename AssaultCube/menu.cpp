#include <Windows.h>
#include <string>
#include <imgui.h>
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "d3d9.h"
#include "draw.h"
#include "settings.h"

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


		SetWindowLong(ownd,
			GWL_EXSTYLE, style);

		//SetWindowPos(ownd, ownd, correct_window_rect.left, correct_window_rect.top, my_heigth, my_width, SWP_SHOWWINDOW);

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

const char* items[]{
	"metric",
	"imperial",
};

void draw_imgui_menu() {

	// convert now to tm struct for UTC
//tm *gmtm = gmtime(&now);
//dt = asctime(gmtm);
	char stamp[256] = "rxr 2021/10/22 menui  ";
	DrawString(stamp, 10, 10, 255, 255, 255, dx_FontCalibri); // Put Main procedure here like ESP etc.


	ImGui::SetNextWindowSize(ImVec2(200, 200));
	//ImGui::SetNextWindowPos(ImVec2(200, 200));
	/*
	if (ImGui::Begin("dbd", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		/*ImGuiWindowFlags_AlwaysAutoResize |*/
		//ImGuiWindowFlags_NoSavedSettings
	//))
	ImGui::Begin("rxr menu");
	{
		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Arduino aimlock##aim", &aim);
		ImGui::SliderInt("Fov", &fov, 0, 180);
		ImGui::Checkbox("ESP##esp", &esp);
		ImGui::Combo("setting ", &item_current, items, IM_ARRAYSIZE(items), 4);
		//ImGui::Checkbox("Aimb##aim", &aim);
		//ImGui::Checkbox("Aimb##aim", &aim);

		ImGui::End();
	}
}
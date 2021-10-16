#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <dwmapi.h>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>

#include "d3d9.h"
#include "draw.h"
#include "menu.h"
#include <imgui.h>
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

HWND hGameWnd;

char lWindowName[256] = "Overlay";
char tWindowName[256] = "AssaultCube"; // put Game window name here
LPCSTR WINDOWNAME = "AssaultCube";
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
MARGINS pMargin;
MSG Message;
HANDLE hnd;
DWORD id;
void entityloop();
int render();

LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam) && show_menu == true)
	{
		return 1l;
	}
	switch (Message)
	{
	case WM_PAINT: // we need to paint? lets paint!
		render();
		break;
	case WM_CREATE:
		return DwmExtendFrameIntoClientArea(hWnd, &pMargin); // extension of window frame into client area
		break;
	case WM_DESTROY:
		PostQuitMessage(0); // We need to use this to exit a message loop
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam); // Making sure all messages are processed
		break;
	}
}
WNDCLASSEXA OverlayWnd; // contains window class information

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
	printf("Debugging Window:\n");


	while (hGameWnd == NULL)
	{
		hGameWnd = FindWindow(0, WINDOWNAME);
		Sleep(1000);
	}
	std::cout << "hwnd: " << hGameWnd << std::endl;

	if (hGameWnd == NULL)
	{
		std::cout << "Window %s Not FOUND Exiting......" << std::endl;

		Sleep(1000);
		exit(0);
	}

	//GetWindowThreadProcessId(hGameWnd, &Kernel::ProcessID);

	//if (Kernel::ProcessID == NULL)
	//	exit(0);

	//std::cout << "processid: " << Kernel::ProcessID << std::endl;


	//DumpEXE(GameModule);

	//std::cout << ("ProcessID: ") << Kernel::ProcessID << std::endl;
	//std::cout << ("GameBase: 0x") << std::uppercase << std::hex << Kernel::GameModule << std::endl;
	//std::cout << "GameBase: " << Kernel::GameModule << std::endl;


	//WNDCLASSEXA OverlayWnd; // contains window class information
	OverlayWnd.cbSize = sizeof(WNDCLASSEXA); // size of struct, basically checking for version or check
	OverlayWnd.style = CS_HREDRAW | CS_VREDRAW;  // Style, redraw method type
	OverlayWnd.lpfnWndProc = Proc; // Pointer to the window procedure
	OverlayWnd.cbClsExtra = 0; // window class struct extra bytes
	OverlayWnd.cbWndExtra = 0; // window instance extra bytes
	OverlayWnd.hInstance = hInstance; // handle to the instance that contains the window procedure for the class
	OverlayWnd.hIcon = LoadIcon(NULL, IDI_APPLICATION); // basic window icon set
	OverlayWnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // basic window icon set
	OverlayWnd.hCursor = LoadCursor(NULL, IDC_ARROW); // basic window cursor icon set
	OverlayWnd.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)); // handle to the class background brush
	OverlayWnd.lpszMenuName = lWindowName;
	OverlayWnd.lpszClassName = lWindowName;


	// registers a window class for the use in call to this createwindowex func
	if (!RegisterClassExA(&OverlayWnd))
	{
		::UnregisterClass(OverlayWnd.lpszClassName, OverlayWnd.hInstance);

		exit(1);
	}

	TargetWnd = FindWindowA(0, tWindowName);
	myhwnd = FindWindowA(0, lWindowName);
	if (TargetWnd)
	{
		GetWindowRect(TargetWnd, &WindowRect);
		//windowWidth = WindowRect.right - WindowRect.left;
		//windowHeight = WindowRect.bottom - WindowRect.top;
		//here make a change
		windowWidth = (WindowRect.right - WindowRect.left);
		windowHeight = (WindowRect.bottom - WindowRect.top);

		DWORD dwStyle = GetWindowLong(TargetWnd, GWL_STYLE);

		if (dwStyle & WS_BORDER) {
			std::cout << "Bordered window need to remove pixels " << std::endl;
			windowWidth -= 6;
			windowHeight -= 29;
		}
		std::cout << "width: " << windowWidth << " " << "height: " << windowHeight << std::endl;
		pMargin = { 0, 0, windowHeight, windowWidth };
		hWnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, lWindowName, lWindowName, WS_POPUP, 1, 1, windowWidth, windowHeight, 0, 0, 0, 0);
	}

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

	ShowWindow(hWnd, /*SW_SHOW*/SW_SHOWDEFAULT);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.DisplaySize.x = clientWidth/*windowWidth*/;
	io.DisplaySize.y = clientHeight/*windowHeight*/;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	D3D9Init(hWnd);


	for (;;)
	{

		if (GetAsyncKeyState(VK_END))
			break;


		if (PeekMessage(&Message, hWnd, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&Message);

			DispatchMessage(&Message);
		}

		TargetWnd = FindWindowA(0, tWindowName);


		if (!TargetWnd)
			exit(0);

		GetWindowRect(TargetWnd, &WindowRect);
		GetClientRect(TargetWnd, &ClientRect);

		windowWidth = WindowRect.right - WindowRect.left;
		windowHeight = WindowRect.bottom - WindowRect.top;


		DWORD dwStyle = GetWindowLong(TargetWnd, GWL_STYLE);

		if (dwStyle & WS_BORDER)
		{
			windowHeight = WindowRect.bottom - WindowRect.top;
			windowWidth = WindowRect.right - WindowRect.left;
			clientHeight = ClientRect.bottom - ClientRect.top;
			clientWidth = ClientRect.right - ClientRect.left;
			borderHeight = (windowHeight - ClientRect.bottom);
			borderWidth = (windowWidth - ClientRect.right) / 2; //only want one side
			borderHeight -= borderWidth; //remove bottom from width, bottom is the same size as either side so we subtract that

			WindowRect.left += borderWidth;
			WindowRect.top += borderHeight;
		}

		MoveWindow(hWnd, WindowRect.left, WindowRect.top, clientWidth, clientHeight, true);

	} // End of Panic Loop

	ImGui::DestroyContext();
	exit(0);

	return 0;
}

bool smooth = false;
void AimAtPos(float x, float y)
{
	//By fredaikis
	float ScreenCenterX = (clientWidth / 2);
	float ScreenCenterY = (clientHeight / 2);
	float TargetX = 0;
	float TargetY = 0;
	float AimSpeed = 1.0f;
	//smooth = float_rand(0.f, 0.8f);
	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}
	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}
	if (!smooth)
	{
		mouse_event(0x0001, (TargetX), (TargetY), NULL, NULL);
		return;
	}

	float r = 5.0;

	TargetX /= r; //10
	TargetY /= r; //10
	if (abs(TargetX) < 1)
	{
		if (TargetX > 0)
		{
			TargetX = 1;
		}
		if (TargetX < 0)
		{
			TargetX = -1;
		}
	}
	if (abs(TargetY) < 1)
	{
		if (TargetY > 0)
		{
			TargetY = 1;
		}
		if (TargetY < 0)
		{
			TargetY = -1;
		}
	}
	mouse_event(0x0001, TargetX, TargetY, NULL, NULL);
}

void entityloop() {



}



int render() {

	dx_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	dx_Device->BeginScene();

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (TargetWnd == GetForegroundWindow())
	{
		time_t now = time(0);
		// convert now to string form
		char* dt = ctime(&now);
		// convert now to tm struct for UTC
		//tm *gmtm = gmtime(&now);
		//dt = asctime(gmtm);
		char stamp[256] = "Shionji ";
		strcat(stamp, dt);
		DrawString(stamp, 10, 10, 255, 0, 255, dx_FontCalibri); // Put Main procedure here like ESP etc.

	}

	//w2s.Update();


	if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
		if (show_menu)
			show_menu = false;
		else
			show_menu = true;
		Sleep(200);
	}

	if (!show_menu) {
		//	Circle((int)(clientWidth / 2), (int)(clientHeight / 2), fov, 0, full, true, 32, BLACK(255));
		entityloop();
		Sleep(1);
	}



	if (show_menu)
	{

		menu();

	}
	else if (windowstate != 0)
	{
		RECT correct_window_rect;
		GetWindowRect(hWnd, &correct_window_rect);

		float my_width = correct_window_rect.right - correct_window_rect.left;
		float my_heigth = correct_window_rect.bottom - correct_window_rect.top;
		//SetWindowPos(hWnd, hWnd, correct_window_rect.left - 20, correct_window_rect.top - 50, my_heigth, my_width, SWP_SHOWWINDOW);
		ChangeClickability(false, /*ohwnd*/hWnd);
	}
	ImGui::EndFrame();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	dx_Device->EndScene();
	dx_Device->PresentEx(0, 0, 0, 0, 0);

	return 0;

}

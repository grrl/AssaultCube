#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <dwmapi.h>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>

#include <thread>

#include "d3d9.h"
#include "draw.h"
#include "menu.h"
#include "driver.h"
#include "offsets.h"
#include "vector2.h"
#include "vector4.h"
#include "w2s.h"
#include "color.h"
#include "serialport.h"

//#include "aimbot.h"

#include <imgui.h>
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

//arduino includes

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cstdio>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Advapi32.lib")

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
float fov = 45;

//arduino

//int id;

//String for getting the output from arduino
char output[MAX_DATA_LENGTH];

/*Portname must contain these backslashes, and remember to
replace the following com port*/
const char *port_name = "\\\\.\\COM3";

SerialPort arduino((char*)port_name);

//String for incoming data
char incomingData[MAX_DATA_LENGTH];

char *c_string;



Offsets* offsets = new Offsets();


auto get_closest_target_to_crosshair(DWORD dw_local_player) -> DWORD
{
	DWORD best_entity = NULL;
	float lowest_distance = FLT_MAX;

	int player_count = Kernel::KeReadVirtualMemory<int>(Kernel::GameModule + offsets->player_count);

	for (auto i = 1; i < player_count; i++)
	{

		DWORD entity_list = Kernel::KeReadVirtualMemory<uintptr_t>(Kernel::GameModule + offsets->entity_list);

		DWORD  entity = Kernel::KeReadVirtualMemory<DWORD>(entity_list + 0x4 * i);

		if (!entity || entity == 14757395258967641292)
			continue;

		bool is_dead = Kernel::KeReadVirtualMemory<int>(entity + offsets->i_dead);

		if (is_dead)
			continue;

		int local_player_team = Kernel::KeReadVirtualMemory<int>(dw_local_player + offsets->i_team);

		int entity_team = Kernel::KeReadVirtualMemory<int>(entity + offsets->i_team);

		if (local_player_team == entity_team)
			continue;

		Vector3 v3_entity_head = Kernel::KeReadVirtualMemory<Vector3>(entity + offsets->v3_head_pos);
		Vector2 v2_entity_screen = get_entity_screen(v3_entity_head);

		Vector3 center_screen;
		center_screen.x = clientWidth / 2;
		center_screen.y = clientHeight / 2;

		float x = v2_entity_screen.x - clientWidth / 2;
		float y = v2_entity_screen.y - clientHeight / 2;
		float f_distance = (float)sqrt((x * x) + (y * y));

		//float f_distance = sqrt(pow((GetSystemMetrics(SM_CXSCREEN) / 2) - v2_entity_screen.x, 2) + pow((GetSystemMetrics(SM_CYSCREEN) / 2) - v2_entity_screen.y, 2));

		if (f_distance < lowest_distance)
		{
			lowest_distance = f_distance;
			best_entity = entity;
		}
	}
	return best_entity;
}

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

auto Aimbot() -> void
{
	while (1)
	{
		// aimbot
		//if (Options::b_aimbot_active)
		//aimbot->start();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}
auto arduino_thread() -> void
{
	/*
	char *c_string = new char[255];
	arduino.readSerialPort(c_string, MAX_DATA_LENGTH);

	std::cout << "data is " << c_string << std::endl;

	delete[] c_string;
	*/

	char *c_string = new char[255];
	arduino.readSerialPort(c_string, MAX_DATA_LENGTH);

	if (c_string == "" || c_string == NULL)
		return;

	std::cout << "data is " << c_string << std::endl;

	delete[] c_string;


	DWORD local_player = Kernel::KeReadVirtualMemory<DWORD>(Kernel::GameModule + offsets->local_player);

	DWORD closest_entity = get_closest_target_to_crosshair(local_player);

	Vector3 head = Kernel::KeReadVirtualMemory<Vector3>(closest_entity + offsets->v3_head_pos);

	head.z += 0.7f;

	Vector2 v2_entity_screen = get_entity_screen(head);

	Vector3 center_screen;
	center_screen.x = clientWidth / 2;
	center_screen.y = clientHeight / 2;
	Vector3 Aimpos;
	Aimpos.x = v2_entity_screen.x;
	Aimpos.y = v2_entity_screen.y;

	float radiusx = (fov) * (center_screen.x / 100.0f);
	float radiusy = (fov) * (center_screen.y / 100.0f);

	if (Aimpos.x >= center_screen.x - radiusx && Aimpos.x <= center_screen.x + radiusx && Aimpos.y >= center_screen.y - radiusy && Aimpos.y <= center_screen.y + radiusy && GetAsyncKeyState(0x46) && 0x8000)
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			int currentx = p.x;
			int currenty = p.y;

			int ourpointx = 960;
			int ourpointy = 540;

			if (currentx == ourpointx && currenty == ourpointy)
				return;

			int moveamountx = 0;
			int moveamounty = 0;
			//cursor position now in p.x and p.y

			if (ourpointx > currentx)
			{
				if (ourpointx - currentx > 127)
				{
					//Console.WriteLine("need -127");
					moveamountx = 127;
				}
				else
				{
					//Console.WriteLine("need -x");
					moveamountx = ourpointx - currentx;
				}
			}
			else if (ourpointx < currentx)
			{
				if (currentx - ourpointx > 127)
				{
					moveamountx = -127;
				}
				else
				{
					moveamountx = -1 * (currentx - ourpointx);
				}
			}

			if (ourpointy > currenty)
			{
				if (ourpointy - currenty > 127)
				{
					//Console.WriteLine("need -127 y");

					moveamounty = 127;
				}
				else
				{
					//Console.WriteLine("need - y");

					moveamounty = ourpointy - currenty;
				}
			}
			else if (ourpointy < currenty)
			{
				if (currenty - ourpointy > 127)
				{
					moveamounty = -127;
				}
				else
				{
					moveamounty = -1 * (currenty - ourpointy);
				}
			}
			std::string coords = "<"", 200>";
			std::string movestring = "<" + std::to_string(moveamountx) + "," + std::to_string(moveamounty) + ">";

			//Creating a c string
			char *c_string = new char[movestring.size()];
			//copying the std::string to c string
			std::copy(movestring.begin(), movestring.end(), c_string);
			//Adding the delimiter
			//c_string[movestring.size()] = '\n';
			//Writing string to arduino
			arduino.writeSerialPort(c_string, /*MAX_DATA_LENGTH*/movestring.size());
			//freeing c_string memory
			delete[] c_string;

			Sleep(10);
		}


		// wrap in condition
	}


	std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
	printf("Debugging Window:\n");

	if (arduino.isConnected())
		std::cout << "Connection Established to Serial...\n";
	else {
		std::cout << "Error, check your COM serial.\n";
		system("pause");
	}

	std::thread thread_render(arduino_thread); // namespace Render
	thread_render.detach();


	Kernel::hKernelDriver = CreateFileA(("\\\\.\\NEET"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	if (Kernel::hKernelDriver == INVALID_HANDLE_VALUE)
	{
		std::cout << "Driver not loaded Exit in 2 Secs..." << std::endl;
		Sleep(2000);
		exit(0);
	}

	std::cout << "handleok: " << Kernel::hKernelDriver << std::endl;

	while (hGameWnd == NULL)
	{
		hGameWnd = FindWindow(0, Kernel::WINDOWNAME);
		Sleep(1000);
	}
	std::cout << "hwnd: " << hGameWnd << std::endl;

	if (hGameWnd == NULL)
	{
		std::cout << "Window %s Not FOUND Exiting......" << std::endl;

		Sleep(1000);
		exit(0);
	}

	GetWindowThreadProcessId(hGameWnd, &Kernel::ProcessID);

	if (Kernel::ProcessID == NULL)
		exit(0);

	std::cout << "processid: " << Kernel::ProcessID << std::endl;

	info_t Input_Output_Data;
	Input_Output_Data.pid = Kernel::ProcessID;
	DWORD Readed_Bytes_Amount;

	DeviceIoControl(Kernel::hKernelDriver, ctl_base, &Input_Output_Data, sizeof Input_Output_Data, &Input_Output_Data, sizeof Input_Output_Data, &Readed_Bytes_Amount, nullptr);
	Kernel::GameModule = (QWORD)Input_Output_Data.data;

	std::cout << "GameBase: " << Kernel::GameModule << std::endl;

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

	//std::thread thread_aimbot(Aimbot);
	//thread_aimbot.detach();
	//std::cout << "[THREAD] Aimbot started!" << std::endl;


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


void arduino_aimbot(DWORD entity, Vector2 v2_entity_screen) {

	Vector3 center_screen;
	center_screen.x = clientWidth / 2;
	center_screen.y = clientHeight / 2;
	Vector3 Aimpos;
	Aimpos.x = v2_entity_screen.x;
	Aimpos.y = v2_entity_screen.y;

	float radiusx = (fov) * (center_screen.x / 100.0f);
	float radiusy = (fov) * (center_screen.y / 100.0f);

	//std::cout << "aimpos " << Aimpos.x << " " << Aimpos.y << "\n";
	if (Aimpos.x >= center_screen.x - radiusx && Aimpos.x <= center_screen.x + radiusx && Aimpos.y >= center_screen.y - radiusy && Aimpos.y <= center_screen.y + radiusy) {
		std::cout << "working " << Aimpos.x << " " << Aimpos.y << "\n";
		//if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000)
		//	AimAtPos(Aimpos.x, Aimpos.y);

		POINT p;
		if (GetCursorPos(&p))
		{
			int currentx = p.x;
			int currenty = p.y;

			int ourpointx = Aimpos.x;
			int ourpointy = Aimpos.y;

			if (currentx == ourpointx && currenty == ourpointy)
				return;

			int moveamountx = 0;
			int moveamounty = 0;
			//cursor position now in p.x and p.y

			if (ourpointx > currentx)
			{
				if (ourpointx - currentx > 127)
				{
					//Console.WriteLine("need -127");
					moveamountx = 127;
				}
				else
				{
					//Console.WriteLine("need -x");
					moveamountx = ourpointx - currentx;
				}
			}
			else if (ourpointx < currentx)
			{
				if (currentx - ourpointx > 127)
				{
					moveamountx = -127;
				}
				else
				{
					moveamountx = -1 * (currentx - ourpointx);
				}
			}

			if (ourpointy > currenty)
			{
				if (ourpointy - currenty > 127)
				{
					//Console.WriteLine("need -127 y");

					moveamounty = 127;
				}
				else
				{
					//Console.WriteLine("need - y");

					moveamounty = ourpointy - currenty;
				}
			}
			else if (ourpointy < currenty)
			{
				if (currenty - ourpointy > 127)
				{
					moveamounty = -127;
				}
				else
				{
					moveamounty = -1 * (currenty - ourpointy);
				}
			}
			std::string coords = "<"", 200>";
			std::string movestring = "<" + std::to_string(moveamountx) + "," + std::to_string(moveamounty) + ">";

			//Creating a c string
			char *c_string = new char[movestring.size()];
			//copying the std::string to c string
			std::copy(movestring.begin(), movestring.end(), c_string);
			//Adding the delimiter
			//c_string[movestring.size()] = '\n';
			//Writing string to arduino
			arduino.writeSerialPort(c_string, /*MAX_DATA_LENGTH*/movestring.size());
			//freeing c_string memory
			delete[] c_string;

			Sleep(1);
		}
	}
}

void aimbot(DWORD entity, Vector2 v2_entity_screen) {

	Vector3 center_screen;
	center_screen.x = clientWidth / 2;
	center_screen.y = clientHeight / 2;
	Vector3 Aimpos;
	Aimpos.x = v2_entity_screen.x;
	Aimpos.y = v2_entity_screen.y;

	float radiusx = (fov) * (center_screen.x / 100.0f);
	float radiusy = (fov) * (center_screen.y / 100.0f);

	//std::cout << "aimpos " << Aimpos.x << " " << Aimpos.y << "\n";
	if (Aimpos.x >= center_screen.x - radiusx && Aimpos.x <= center_screen.x + radiusx && Aimpos.y >= center_screen.y - radiusy && Aimpos.y <= center_screen.y + radiusy) {
		std::cout << "working " << Aimpos.x << " " << Aimpos.y << "\n";
		if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000)
			AimAtPos(Aimpos.x, Aimpos.y);

	}

}

void entityloop() {

	//int player_count = offsets->get_player_count();
	//process->read<int>(process->dw_module + p_game->player_count);

	int player_count = Kernel::KeReadVirtualMemory<int>(Kernel::GameModule + offsets->player_count);

	std::cout << "playercount " << player_count << "\n";

	DWORD local_player = Kernel::KeReadVirtualMemory<DWORD>(Kernel::GameModule + offsets->local_player);

	//DWORD closest_entity = get_closest_target_to_crosshair(local_player);

	for (auto i = 1; i < player_count; i++) {

		if (!local_player)
			continue;

		//std::cout << "local_player " << local_player << "\n";

		DWORD entity_list = Kernel::KeReadVirtualMemory<uintptr_t>(Kernel::GameModule + offsets->entity_list);

		//std::cout << "entity_list " << entity_list << "\n";

		DWORD  entity = Kernel::KeReadVirtualMemory<DWORD>(entity_list + 0x4 * i);


		if (!entity || entity == 14757395258967641292)
			continue;

		std::cout << "entity success " << entity << "\n";

		bool is_dead = Kernel::KeReadVirtualMemory<int>(entity + offsets->i_dead);

		std::cout << "is_dead " << is_dead << "\n";

		if (is_dead)
			continue;

		int local_player_team = Kernel::KeReadVirtualMemory<int>(local_player + offsets->i_team);

		std::cout << "local_player_team " << local_player_team << "\n";

		int entity_team = Kernel::KeReadVirtualMemory<int>(entity + offsets->i_team);

		std::cout << "entity_team " << entity_team << "\n";

		if (local_player_team == entity_team)
			continue;

		Vector3 head = Kernel::KeReadVirtualMemory<Vector3>(entity + offsets->v3_head_pos);
		Vector3 foot = Kernel::KeReadVirtualMemory<Vector3>(entity + offsets->v3_foot_pos);


		head.z += 0.7f;

		Vector2 w2s_head = get_entity_screen(head);
		Vector2 w2s_foot = get_entity_screen(foot);

		std::cout << "w2s_foot x " << w2s_foot.x << " " << w2s_foot.y <<"\n";

		if (w2s_head == Vector2(-1, -1) || w2s_foot == Vector2(-1, -1))
			continue;

		/*
		if (entity == closest_entity) {
			//aimbot(entity, w2s_head);
			arduino_aimbot(entity, w2s_head);
		}
		*/
		//startcopypaste

		if (entity_team == local_player_team) {

			int health = Kernel::KeReadVirtualMemory<int>(entity + offsets->i_health);

			std::cout << "health is " << health << "\n";

			float Height = w2s_head.y - w2s_foot.y;
			float delta = (Height / 100.0f) * health;

			if (health > 75)
				DrawHealthBar((w2s_foot.x - Height / 4) + 3, w2s_foot.y, 3, delta, 57, 255, 20, 255);
			else if (health > 50)
				DrawHealthBar((w2s_foot.x - Height / 4) + 3, w2s_foot.y, 3, delta, 255, 255, 0, 255);
			else if (health > 25)
				DrawHealthBar((w2s_foot.x - Height / 4) + 3, w2s_foot.y, 3, delta, 255, 102, 0, 255);
			else
				DrawHealthBar((w2s_foot.x - Height / 4) + 3, w2s_foot.y, 3, delta, 255, 0, 0, 255);

			float rMy = 0.392;
			float gMy = 0.584;
			float bMy = 0.930;
			float aMy = 1;

			DrawLine(w2s_foot.x - Height / 4, w2s_head.y, w2s_foot.x - Height / 4, w2s_head.y - Height / 5, rMy * 255, gMy * 255, bMy * 255, aMy * 255);
			DrawLine(w2s_foot.x - Height / 4, w2s_foot.y, w2s_foot.x - Height / 4, w2s_foot.y + Height / 5, rMy * 255, gMy * 255, bMy * 255, aMy * 255);

			DrawLine(w2s_foot.x + Height / 4, w2s_head.y, w2s_foot.x + Height / 4, w2s_foot.y - Height / 5, rMy * 255, gMy * 255, bMy * 255, aMy * 255);
			DrawLine(w2s_foot.x + Height / 4, w2s_foot.y, w2s_foot.x + Height / 4, w2s_foot.y + Height / 5, rMy * 255, gMy * 255, bMy * 255, aMy * 255);

			DrawLine(w2s_foot.x - Height / 4, w2s_head.y, w2s_foot.x - Height / 16, w2s_head.y, rMy * 255, gMy * 255, bMy * 255, aMy * 255);
			DrawLine(w2s_foot.x + Height / 4, w2s_head.y, w2s_foot.x + Height / 16, w2s_head.y, rMy * 255, gMy * 255, bMy * 255, aMy * 255);

			DrawLine(w2s_foot.x - Height / 4, w2s_foot.y, w2s_foot.x - Height / 16, w2s_foot.y, rMy * 255, gMy * 255, bMy * 255, aMy * 255);
			DrawLine(w2s_foot.x + Height / 4, w2s_foot.y, w2s_foot.x + Height / 16, w2s_foot.y, rMy * 255, gMy * 255, bMy * 255, aMy * 255);

			/*
			char buffer[5];
			char buffer2[4];
			//double dist2 = dist >= 0. ? floor(dist*100.) / 100. : ceil(dist*100.) / 100.;
			int ret = snprintf(buffer, sizeof buffer, "%f", Round(dist));
			int ret2 = snprintf(buffer2, sizeof buffer2, "%f", health);
			//printf("%d\n", dist);
			char printChar1[2] = "[";
			char printChar2[4] = "m] ";
			char result[15];   // array to hold the result.

			strcpy(result, printChar1); // copy string one into the result.
			strcat(result, buffer); // append string two to the result.
			strcat(result, printChar2); // append string two to the result.
			strcat(result, buffer2); // append string two to the result.
			DrawShadowString(result, entity_transformed.x, entity_transformed.y, 255, 255, 255, dx_FontCalibri);
			*/
			//write_glow_team(entity);
		}
		else {

			int health = Kernel::KeReadVirtualMemory<int>(entity + offsets->i_health);

			std::cout << "health is " << health << "\n";

			float Height = w2s_head.y - w2s_foot.y;
			float delta = (Height / 100.0f) * health;

			if (health > 75)
				DrawHealthBar((w2s_foot.x - Height / 4) + 3, w2s_foot.y, 3, delta, 57, 255, 20, 255);
			else if (health > 50)
				DrawHealthBar((w2s_foot.x - Height / 4) + 3, w2s_foot.y, 3, delta, 255, 255, 0, 255);
			else if (health > 25)
				DrawHealthBar((w2s_foot.x - Height / 4) + 3, w2s_foot.y, 3, delta, 255, 102, 0, 255);
			else
				DrawHealthBar((w2s_foot.x - Height / 4) + 3, w2s_foot.y, 3, delta, 255, 0, 0, 255);

			float rEn = 1;
			float gEn = 0;
			float bEn = 0;
			float aEn = 1;

			//if (dist <= 200.f)
			//	DrawLine(clientWidth / 2, clientHeight / 2, entity_transformed.x, entityhead_transformed.y, 57, 255, 20, 255);


			DrawLine(w2s_foot.x - Height / 4, w2s_head.y, w2s_foot.x - Height / 4, w2s_head.y - Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
			DrawLine(w2s_foot.x - Height / 4, w2s_foot.y, w2s_foot.x - Height / 4, w2s_foot.y + Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

			DrawLine(w2s_foot.x + Height / 4, w2s_head.y, w2s_foot.x + Height / 4, w2s_head.y - Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
			DrawLine(w2s_foot.x + Height / 4, w2s_foot.y, w2s_foot.x + Height / 4, w2s_foot.y + Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

			DrawLine(w2s_foot.x - Height / 4, w2s_head.y, w2s_foot.x - Height / 16, w2s_head.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
			DrawLine(w2s_foot.x + Height / 4, w2s_head.y, w2s_foot.x + Height / 16, w2s_head.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

			DrawLine(w2s_foot.x - Height / 4, w2s_foot.y, w2s_foot.x - Height / 16, w2s_foot.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
			DrawLine(w2s_foot.x + Height / 4, w2s_foot.y, w2s_foot.x + Height / 16, w2s_foot.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

			/*
			char buffer[5];
			char buffer2[4];
			//double dist2 = dist >= 0. ? floor(dist*100.) / 100. : ceil(dist*100.) / 100.;
			int ret = snprintf(buffer, sizeof buffer, "%f", Round(dist));
			int ret2 = snprintf(buffer2, sizeof buffer2, "%f", health);
			//printf("%d\n", dist);
			char printChar1[2] = "[";
			char printChar2[4] = "m] ";
			char result[15];   // array to hold the result.

			strcpy(result, printChar1); // copy string one into the result.
			strcat(result, buffer); // append string two to the result.
			strcat(result, printChar2); // append string two to the result.
			strcat(result, buffer2); // append string two to the result.
			DrawShadowString(result, entity_transformed.x, entity_transformed.y, 255, 255, 255, dx_FontCalibri);
			*/
	
		}

		//endcopypaste

	}

}
int render() {

	//char *c_string = new char[255];

	//arduino.readSerialPort(c_string, MAX_DATA_LENGTH);

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
		Circle((int)(clientWidth / 2), (int)(clientHeight / 2), fov, 0, full, true, 32, BLACK(255));
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

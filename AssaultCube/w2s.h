#include "driver.h"
#include "d3d9.h"
#include "offsets.h"
#include "structs.h"
#include "vector4.h"

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

ViewMatrix get_view_matrix()
{
	return Kernel::KeReadVirtualMemory<ViewMatrix>(Kernel::GameModule + offsets->view_matrix);
}

auto world_to_screen(Vector3 v3_world, ViewMatrix matrix, int width, int height) -> Vector2
{
	Vector4 convert;
	convert.x = (v3_world.x * matrix.vm0) + (v3_world.y * matrix.vm4) + (v3_world.z * matrix.vm8) + matrix.vm12;
	convert.y = (v3_world.x * matrix.vm1) + (v3_world.y * matrix.vm5) + (v3_world.z * matrix.vm9) + matrix.vm13;
	convert.z = (v3_world.x * matrix.vm2) + (v3_world.y * matrix.vm6) + (v3_world.z * matrix.vm10) + matrix.vm14; // not used
	convert.w = (v3_world.x * matrix.vm3) + (v3_world.y * matrix.vm7) + (v3_world.z * matrix.vm11) + matrix.vm15;

	if (convert.w < 0.001f)
		return Vector2(-1, -1);

	Vector2 cam;
	cam.x = width / 2.0f;
	cam.y = height / 2.0f;

	Vector2 screen;
	screen.x = cam.x + (cam.x * convert.x / convert.w);
	screen.y = cam.y - (cam.y * convert.y / convert.w);

	return screen;
}

auto get_entity_screen(Vector3 v3_world) -> Vector2
{
	ViewMatrix f_matrix;

	f_matrix = Kernel::KeReadVirtualMemory<ViewMatrix>(Kernel::GameModule + offsets->view_matrix);

	std::cout << "matrix is " << f_matrix.vm0 << " " << f_matrix.vm1 << "\n";
	/*
	for (auto i = 0; i < 16; i++) // index is always 16 in 3D games
	{
		f_matrix[i] = Kernel::KeReadVirtualMemory<float>(Kernel::GameModule + offsets->view_matrix + 0x4 * i);
	}
	*/

	return world_to_screen(v3_world, f_matrix, windowWidth, windowHeight);
}

DWORD get_closest_target_to_crosshair(DWORD dw_local_player)
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
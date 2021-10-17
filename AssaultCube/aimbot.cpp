#include "aimbot.h"
#include "driver.h"
#include "vector2.h"
#include "offsets.h"
#include "w2s.h"


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
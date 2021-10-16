#include "driver.h"
#include "d3d9.h"
#include "offsets.h"
#include "structs.h"

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
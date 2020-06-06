#pragma once
#include <windows.h>
#include <vector>
using namespace std;
typedef struct cga_map_cells_s
{
	cga_map_cells_s()
	{
		x_bottom = 0;
		y_bottom = 0;
		xsize = 0;
		ysize = 0;
	}
	cga_map_cells_s(int a1, int a2, int a3, int a4, short *a5) : x_bottom(a1), y_bottom(a2), xsize(a3), ysize(a4)
	{
		cells.resize(a3 * a4);
		memcpy(cells.data(), a5, sizeof(short) * a3 * a4);
	}
	int x_bottom;
	int y_bottom;
	int xsize;
	int ysize;
	std::vector<short> cells;
}cga_map_cells_t;
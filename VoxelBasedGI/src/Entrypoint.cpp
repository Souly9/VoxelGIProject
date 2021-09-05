#include "stdafx.h"
#include "VoxelBasedGIApp.h"

int main()
{
	int x, y;
	std::cout << "Enter window width (Recommended: 1920) \n";
	std::cin >> x;
	std::cout << "Enter window height (Recommended: 1080) \n";
	std::cin >> y;
	VoxelBasedGIApp prog("VoxelBasedGI", x, y);
	prog.Run();
	return 1;
}

#pragma once
#include <GLFW/glfw3.h>

struct DestroyglfwWin
{
	void operator()(GLFWwindow* ptr)
	{
		glfwDestroyWindow(ptr);
	}
};

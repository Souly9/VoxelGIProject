#include "stdafx.h"
#include "InputManager.h"

bool InputManager::m_rightMouseButtonPressed = false;
bool InputManager::m_leftMouseButtonPressed = false;
bool InputManager::m_WASDPressed = false;

float InputManager::m_moveSensitivity = 0.1f;

void InputManager::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	MAYBE_UNUSED(window);
	if (m_rightMouseButtonPressed)
	{
		SignalSystem::InputSignals::Get()->onMouseDrag(glm::vec2(ComputeX(xpos), ComputeY(ypos)));
	}
	if (m_leftMouseButtonPressed)
	{
		SignalSystem::InputSignals::Get()->onMouseLeftDrag(glm::vec2{xpos, ypos});
	}
}

void InputManager::OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	MAYBE_UNUSED(window);
	MAYBE_UNUSED(xoffset);
	SignalSystem::InputSignals::Get()->onMouseScroll(-yoffset);
}

void InputManager::OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	using namespace SignalSystem;
	MAYBE_UNUSED(window);
	MAYBE_UNUSED(mods);
	MAYBE_UNUSED(scancode);
	// M & N switch between lines and filled polygons
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Keyboard movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		InputSignals::Get()->onWASDPress(glm::vec4{0, 1 * m_moveSensitivity, 0, 0});
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		InputSignals::Get()->onWASDPress(glm::vec4{0, -1 * m_moveSensitivity, 0, 0});
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		InputSignals::Get()->onWASDPress(glm::vec4{0, 0, 1 * m_moveSensitivity, 0});
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		InputSignals::Get()->onWASDPress(glm::vec4{0, 0, -1 * m_moveSensitivity, 0});
	}
}

double InputManager::ComputeY(double yPos)
{
	return WindowManager::Get()->GetScreenHeight() - yPos;
}

double InputManager::ComputeX(double xPos)
{
	return WindowManager::Get()->GetScreenWidth() - xPos;
}

void InputManager::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	MAYBE_UNUSED(mods);
	using namespace SignalSystem;
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		m_rightMouseButtonPressed = true;
		InputSignals::Get()->onMouseRightClick(glm::vec2(ComputeX(xpos), ComputeY(ypos)));
		InputSignals::Get()->onMouseStartRightDrag();
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		m_rightMouseButtonPressed = false;
		InputSignals::Get()->onMouseStopRightDrag();
	}

	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		InputSignals::Get()->onMouseLeftClick(glm::vec2{xpos, ypos});
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			m_leftMouseButtonPressed = true;
			InputSignals::Get()->onMouseStartLeftDrag(glm::vec2{xpos, ypos});
		}
	}

	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		m_leftMouseButtonPressed = false;
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			InputSignals::Get()->onMouseStopLeftDrag(glm::vec2{xpos, ypos});
		}
	}
}

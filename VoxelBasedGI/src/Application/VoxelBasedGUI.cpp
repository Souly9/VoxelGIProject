#include "stdafx.h"
#include "VoxelBasedGUI.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

VoxelBasedGUI::VoxelBasedGUI(GLFWwindow* window)
{
	SignalSystem::ApplicationEvents::Get()->onUpdate.connect([this](auto deltaTime) { Update(deltaTime); });
	// Create ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	ImGui::StyleColorsDark();
}

VoxelBasedGUI::~VoxelBasedGUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void VoxelBasedGUI::Update(float delta)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Update frame data
	m_lastFrameTime = delta;
	m_currentFPS = 1.0f / delta;
}

void VoxelBasedGUI::DrawGUI()
{
	ImGui::Begin("VoxelGI Settings");
	ImGui::Text("Scene Selection");
	m_shouldSceneChange = false;
	if (ImGui::Button("Cornell Classic"))
	{
		m_shouldSceneChange = true;
		m_currentScene = 0;
	}
	if (ImGui::Button("Cornell + Bunny"))
	{
		m_shouldSceneChange = true;
		m_currentScene = 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cornell Spheres"))
	{
		m_shouldSceneChange = true;
		m_currentScene = 2;
	}
	if (ImGui::Button("Cornell + Spheres + Lucy"))
	{
		m_shouldSceneChange = true;
		m_currentScene = 3;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cornell + Lucy"))
	{
		m_shouldSceneChange = true;
		m_currentScene = 4;
	}
	if (ImGui::Button("Cornell Water"))
	{
		m_shouldSceneChange = true;
		m_currentScene = 5;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cornell Water + Lucy"))
	{
		m_shouldSceneChange = true;
		m_currentScene = 6;
	}

	ImGui::Text("Voxel Grid Resolution");

	if (ImGui::Button("64"))
		m_currentVoxelRes = 64;
	ImGui::SameLine();
	if (ImGui::Button("128"))
		m_currentVoxelRes = 128;
	ImGui::SameLine();
	if (ImGui::Button("256"))
		m_currentVoxelRes = 256;
	ImGui::SameLine();
	if (ImGui::Button("512"))
		m_currentVoxelRes = 512;

	ImGui::SliderFloat("Voxel Grid Size", &m_currentSceneSize, 1, 300);
	ImGui::Checkbox("Update Voxel GI", &m_updateGI);
	ImGui::SliderInt("Voxelize Every X Frames", &m_framesBetweenRevoxelization, 1, 240);
	ImGui::SliderFloat("Diffuse Step Size Multiplier", &m_stepSize, 0.01f, 3.0f);
	ImGui::SliderFloat("Diffuse/Shadow Cone Max Distance", &m_coneTracingMaxDist, 0.0f, 3.0f);
	ImGui::SliderFloat("Diffuse Cone Angle in Rad", &m_coneAngle, 0.0f, 3.0f);

	if (ImGui::CollapsingHeader("General Lighting Settings"))
	{
		ImGui::Text("Roughness Source");
		if (ImGui::Button("Material"))
			m_globalLighting.w = 0;
		ImGui::SameLine();
		if (ImGui::Button("Custom"))
			m_globalLighting.w = 1;
		ImGui::SliderFloat("Custom Roughness", &m_globalLighting.y, 0.05f, 1.0f);

		ImGui::SliderFloat("Exposure", &m_globalLighting.x, 0.0f, 8.0f);

		ImGui::Text("Tonemapper");
		if (ImGui::Button("Modified Reinhard"))
			m_globalLighting.z = 0;
		ImGui::SameLine();
		if (ImGui::Button("ACES"))
			m_globalLighting.z = 1;
		ImGui::SameLine();
		if (ImGui::Button("Uncharted 2"))
			m_globalLighting.z = 2;
	}
	if (ImGui::CollapsingHeader("Shadow Cone Settings"))
	{
		ImGui::SliderFloat("First Weight", &m_shadowSettings.z, 0.0f, 1.0f);
		ImGui::SliderFloat("Second Weight", &m_shadowSettings.w, 0.0f, 1.0f);
	}
	if (ImGui::CollapsingHeader("Specular Cone Settings"))
	{
		ImGui::SliderFloat("Specular Cone Angle in Rad", &m_specularSettings.y, 0.0f, 1.0f);
		ImGui::SliderFloat("Step Size Multiplier", &m_specularSettings.z, 0.0f, 1.0f);
		ImGui::SliderFloat("Trace Distance", &m_specularSettings.w, 0.0f, 7.0f);
	}
	ImGui::Checkbox("Compute Second LightBounce", &m_computeSecondLightBounce);
	ImGui::Checkbox("Show VoxelGI Debug", &m_showDebug);
	ImGui::Checkbox("Enable Voxel GI", &m_showGI);
	static char fpsBuffer[7];
	snprintf(fpsBuffer, 7, "%f", m_currentFPS);
	ImGui::Text("Current FPS: ");
	ImGui::SameLine();
	ImGui::Text(fpsBuffer);
	snprintf(fpsBuffer, 7, "%f", m_lastFrameTime * 1000.0f);
	//std::cout << fpsBuffer<<'\n';
	ImGui::Text("Frame Time: ");
	ImGui::SameLine();
	ImGui::Text(fpsBuffer);
	ImGui::SameLine();
	ImGui::Text("ms");
	ImGui::Text("Currently Rendering Deferred");
	ImGui::Text("Ctrl + Mouse: Look Around!");
	ImGui::Text("N / M: Switch Between Wireframe / Shaded");
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool VoxelBasedGUI::ShouldComputeSecondLightBounce() const
{
	return m_computeSecondLightBounce;
}

bool VoxelBasedGUI::ShouldUpdateGI() const
{
	return m_updateGI;
}

bool VoxelBasedGUI::ShouldChangeScene() const
{
	return m_shouldSceneChange;
}

int VoxelBasedGUI::GetSceneIndex() const
{
	return m_currentScene;
}

glm::vec4 VoxelBasedGUI::GetShadowSettings() const
{
	return m_shadowSettings;
}

glm::vec4 VoxelBasedGUI::GetSpecularSettings() const
{
	return m_specularSettings;
}

glm::vec4 VoxelBasedGUI::GetLightingSettings() const
{
	return m_globalLighting;
}

int VoxelBasedGUI::GetVoxelRes() const
{
	return m_currentVoxelRes;
}

float VoxelBasedGUI::GetVoxelSize() const
{
	return m_currentSceneSize;
}

int VoxelBasedGUI::GetFramesBetweenVoxelization() const
{
	return m_framesBetweenRevoxelization;
}

bool VoxelBasedGUI::ShowDebug() const
{
	return m_showDebug;
}

bool VoxelBasedGUI::GetShowGI() const
{
	return m_showGI;
}

float VoxelBasedGUI::GetStepSize() const
{
	return m_stepSize;
}

float VoxelBasedGUI::GetMaxDist() const
{
	return m_coneTracingMaxDist;
}

float VoxelBasedGUI::GetConeAngle() const
{
	return m_coneAngle;
}

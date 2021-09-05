#pragma once

struct GLFWwindow;

class VoxelBasedGUI
{
public:
	VoxelBasedGUI(GLFWwindow* window);
	~VoxelBasedGUI();

	void Update(float delta);
	void DrawGUI();

	/*
	 * -----------------------------------------------------------------------
	 * Getters for GUI params
	 * -----------------------------------------------------------------------
	 */
	int GetVoxelRes() const;
	float GetVoxelSize() const;
	int GetFramesBetweenVoxelization() const;
	bool ShouldComputeSecondLightBounce() const;
	bool ShowDebug() const;
	bool GetShowGI() const;
	bool ShouldUpdateGI() const;
	bool ShouldChangeScene() const;
	int GetSceneIndex() const;
	float GetStepSize() const;
	float GetMaxDist() const;
	float GetConeAngle() const;
	glm::vec4 GetShadowSettings() const;
	glm::vec4 GetSpecularSettings() const;
	glm::vec4 GetLightingSettings() const;
private:
	float m_currentFPS;
	float m_lastFrameTime;
	int m_framesBetweenRevoxelization{50};
	float m_stepSize{1.0f};
	float m_coneTracingMaxDist{1.6f};
	float m_coneAngle{1.04f};
	// z = firstBlend
	// w = secondBlend
	glm::vec4 m_shadowSettings{1.0, 3, 0.2f, 0.14f};
	// y = Cone Angle
	// z = Step Size
	// w = Distance
	glm::vec4 m_specularSettings{1, 0.005f, 1.f, 2.9f};
	// x = Exposure
	// y = Custom Roughness
	// z = Tone Map Operator
	// w = Roughness Source Mode
	glm::vec4 m_globalLighting{1, 0.5, 2, 0};
	int m_currentVoxelRes{VOXEL_INITIAL_DIMENSIONS};
	int m_currentScene{0};
	float m_currentSceneSize{2.5f};
	bool m_shouldSceneChange;
	bool m_showDebug{false};
	bool m_showGI{false};
	bool m_updateGI{true};
	bool m_computeSecondLightBounce{true};
};

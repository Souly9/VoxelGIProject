#pragma once
#include <stdafx.h>

#include "UniformBuffer.h"

class Camera;
class LightManager;

class Scene
{
public:
	Scene(std::shared_ptr<Camera> cam);

	/*
	 * Submits uniform buffer filled with all scene relevant information to shaders
	 */
	void BeginScene();

	void PrepareBuffer();

	std::unique_ptr<LightManager>& GetLightManager() noexcept { return m_pLightManager; }

	glm::mat4 GetViewMat() const noexcept { return m_data.m_viewMatrix; }
protected:
	struct SceneUniformBuffer
	{
		SceneUniformBuffer() = default;

		glm::mat4 m_projMatrix{};
		glm::mat4 m_viewMatrix{};
		float m_totalTime;
		float m_deltaTime;
	};

	SceneUniformBuffer m_data;

	UniformBuffer<SceneUniformBuffer> m_buffer;
	std::unique_ptr<LightManager> m_pLightManager;
	std::shared_ptr<Camera> m_pMainCamera;

	bool m_isDirty;
};

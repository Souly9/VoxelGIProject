#include "stdafx.h"
#include "Scene.h"
#include "UniformBuffer.h"

#include <utility>


Scene::Scene(std::shared_ptr<Camera> cam) : m_buffer(COMBUSTION_SCENE_BUFFER_INDEX),
                                            m_pLightManager(
	                                            std::make_unique<LightManager>(COMBUSTION_AMBIENT_STRENGTH)),
                                            m_pMainCamera(std::move(cam)), m_isDirty(true)
{
}

void Scene::BeginScene()
{
	if (m_isDirty)
	{
		PrepareBuffer();
		m_buffer.UpdateBuffer(m_data);
		m_pLightManager->SubmitLightBuffer();
	}
}

void Scene::PrepareBuffer()
{
	m_data.m_viewMatrix = m_pMainCamera->GetViewMatrix();
	m_data.m_projMatrix = m_pMainCamera->GetProjectionMatrix();
	DataHolder::Get()->GetTimes(m_data.m_totalTime, m_data.m_deltaTime);
}

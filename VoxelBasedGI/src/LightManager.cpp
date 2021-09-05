#include "stdafx.h"
#include "LightManager.h"

LightManager::LightManager(float ambientStr) :
	m_lightBufferData(ambientStr),
	m_buffer(
		COMBUSTION_GENERIC_LIGHT_BUFFER_INDEX, (sizeof(glm::vec4) * 2 + sizeof(PointLight) * COMBUSTION_MAX_LIGHTS)),
	m_isDirty(true)
{
	SignalSystem::ApplicationEvents::Get()->onMainCameraValsChanged.connect([this]()
	{
		m_isDirty = true;
	});
}

void LightManager::AddLight(PointLight& light)
{
	m_pointLights.push_back(light);
	m_isDirty = true;
}

void LightManager::AddLights(std::vector<PointLight> lights)
{
	for (int i = 0; i < lights.size(); ++i)
	{
		m_pointLights.push_back(lights[i]);
	}
	m_isDirty = true;
}

void LightManager::RemoveLight(PointLight light)
{
	for (int i = 0; i < m_pointLights.size(); ++i)
	{
		if (&m_pointLights.at(i) == &light)
		{
			m_pointLights.erase(m_pointLights.begin() + i);
		}
	}
	m_isDirty = true;
	SubmitLightBuffer();
}

void LightManager::Clear()
{
	for (int i = m_pointLights.size() - 1; i >= 0; --i)
	{
		m_buffer.UpdateBuffer(sizeof(glm::vec4) + sizeof(glm::vec4) + (i * sizeof(PointLight)),
		                      PointLight{glm::vec4{0}, glm::vec4{0}});
		m_pointLights.erase(m_pointLights.begin() + i);
	}
}

void LightManager::RemoveAtIndex(int index)
{
	if (index < m_pointLights.size())
	{
		m_pointLights.erase(m_pointLights.begin() + index);
		m_isDirty = true;
	}
}

void LightManager::SubmitLightBuffer()
{
	if (m_isDirty)
	{
		m_lightBufferData.m_cameraPos = glm::vec4{DataHolder::Get()->GetCameraRaw()->GetPosition(), 1};
		m_buffer.UpdateBuffer(0, m_lightBufferData.m_ambientStrength);
		m_buffer.UpdateBuffer(sizeof(glm::vec4), m_lightBufferData.m_cameraPos);
		m_buffer.UpdateBuffer(sizeof(glm::vec4) + sizeof(glm::vec4), m_pointLights);
		m_isDirty = false;
	}
}

void LightManager::MultiplyLights(glm::mat4 mat)
{
	m_pointLights.clear();
	for (int i = 0; i < m_pointLights.size(); ++i)
	{
		const PointLight& light = m_pointLights.at(i);
		m_pointLights.push_back(
			PointLight{mat * light.m_position, light.m_color});
	}
	m_isDirty = true;
}

#pragma once
#include "LightStructs.h"
#include "UniformBuffer.h"

class ShaderResource;

class LightManager
{
public:
	LightManager(float ambientStr);

	void AddLight(PointLight& light);
	void AddLights(std::vector<PointLight> lights);

	void RemoveLight(PointLight light);
	void Clear();
	void RemoveAtIndex(int index);

	void SubmitLightBuffer();

	void MultiplyLights(glm::mat4 mat);
private:
	struct LightData
	{
		LightData(float ambient) : m_ambientStrength(ambient)
		{
		}

		glm::vec4 m_ambientStrength;
		glm::vec4 m_cameraPos;
	};

	std::vector<PointLight> m_pointLights;

	LightData m_lightBufferData;
	UniformBuffer<LightData> m_buffer;

	bool m_isDirty;
};

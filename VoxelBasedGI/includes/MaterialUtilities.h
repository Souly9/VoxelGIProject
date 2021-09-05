#pragma once
#include <map>
#include <glm/vec3.hpp>

#include "TextureManager.h"

struct TextureData
{
public:
	TextureData(const TextureData& data) = default;
	TextureData() = default;

	~TextureData()
	{
		uint64_t texs[5]{m_albedo, m_displacement, m_normalMap, m_cubeMap, m_specularMap};
		TextureManager::Get()->ReleaseTexturesARB(5, texs);
	}

	uint64_t m_albedo;
	uint64_t m_displacement;
	uint64_t m_normalMap;
	uint64_t m_cubeMap;
	uint64_t m_specularMap;
};

struct Properties
{
public:
	Properties() = default;

	Properties(const glm::vec3 ambient,
	           const glm::vec3 diffuse,
	           const glm::vec3 emissive,
	           const float shininess) noexcept : m_ambient(ambient), m_diffuse(diffuse), m_emissive(emissive),
	                                             m_shininess(shininess)
	{
	}

	glm::vec3 m_ambient;
	glm::vec3 m_diffuse;
	glm::vec3 m_emissive;
	float m_shininess;
};

namespace MaterialUtilities
{
	struct PresetConverter;
	static std::unique_ptr<PresetConverter> m_pSingleton;


	enum class MaterialPreset
	{
		Default
	};

	enum class ShaderAttribute
	{
		NORMALS,
		TEXCOORDS,
		VERTEXCOLOR,
		TANGENTSPACE
	};

	enum class UniformAttribute
	{
		ALBEDO,
		NORMALMAP,
		DISPLACEMENTMAP,
		CUBEMAP,
		SPECULARMAP
	};

	struct PresetConverter
	{
	public:
		PresetConverter() = default;

		static PresetConverter* Get()
		{
			if (!m_pSingleton)
			{
				m_pSingleton = std::make_unique<PresetConverter>();
			}
			return m_pSingleton.get();
		}

		std::vector<ShaderAttribute> ConvertToSA(MaterialPreset preset)
		{
			return m_convertToSAMap[preset];
		}

		std::vector<UniformAttribute> ConvertToUA(MaterialPreset preset)
		{
			return m_convertToUAMap[preset];
		}

	private:
		PresetConverter(const PresetConverter& conv) = delete;

		std::map<MaterialPreset, std::vector<ShaderAttribute>> m_convertToSAMap;
		std::map<MaterialPreset, std::vector<UniformAttribute>> m_convertToUAMap;
	};
}

#pragma once
#include "Shader.h"

class Shader;
struct TextureData;

class Material
{
public:
	Material() : m_myShaderProgram()
	{
	};
	Material(MaterialUtilities::MaterialPreset def, Shader& shader, uint32_t id);

	Material(MaterialUtilities::MaterialPreset def,
	         const Shader& shader,
	         uint32_t id,
	         const Properties& props,
	         std::shared_ptr<TextureData> handles,
	         std::vector<MaterialUtilities::ShaderAttribute> attbs);
	Material(MaterialUtilities::MaterialPreset def, const Shader& shader, uint32_t id,
	         const Properties& props, std::shared_ptr<TextureData> handles,
	         std::vector<MaterialUtilities::ShaderAttribute> attbs,
	         std::vector<MaterialUtilities::UniformAttribute> uniforms);

	bool operator==(const Material& mat) const noexcept
	{
		return m_myShaderProgram.m_ID == mat.m_myShaderProgram.m_ID;
	}

	std::vector<MaterialUtilities::ShaderAttribute> GetShaderAttributes() const { return m_shaderAttributes; }
	std::vector<MaterialUtilities::UniformAttribute> GetUniformAttributes() const { return m_uniformAttributes; }

	uint32_t GetID() const noexcept
	{
		return m_ID;
	}

	void BindShader()
	{
		m_myShaderProgram.Use();
	}

	Shader GetShader() const noexcept
	{
		return m_myShaderProgram;
	}

	void SetShader(Shader shader)
	{
		m_myShaderProgram = shader;
	}

	std::shared_ptr<TextureData> GetHandles() const noexcept;
	void BindProperties();
private:
	uint32_t m_ID;
	Shader m_myShaderProgram;


	Properties m_properties;
	std::vector<MaterialUtilities::ShaderAttribute> m_shaderAttributes;
	std::vector<MaterialUtilities::UniformAttribute> m_uniformAttributes;


	std::shared_ptr<TextureData> m_textures;
};

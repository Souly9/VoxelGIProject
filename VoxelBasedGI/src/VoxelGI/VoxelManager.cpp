#include "stdafx.h"
#include "VoxelGI/VoxelManager.h"
#include <random>
#include "Renderer.h"

VoxelManager::VoxelManager(uint32_t voxelDimensions)
	: m_voxelUniformData(),
	  m_voxelUniformBuffer(COMBUSTION_VOXEL_UNIFORM_BUFFER),
	  m_coneTraceUniformBuffer(COMBUSTION_CONE_TRACING_BUFFER_INDEX)
{
	using namespace VoxelIncludeConstants;

	// Create shaders with the previously defined include directories 
	m_voxelizeMaterial = Shader("Shaders/VoxelBasedGI/Voxelization.vert",
	                            "Shaders/VoxelBasedGI/Voxelization.frag",
	                            "Shaders/VoxelBasedGI/Voxelization.geom",
	                            coneTracingNumOfIncludes, coneTracingIncludes, coneTracingLengths);

	m_debugVoxelMaterial = Shader("Shaders/VoxelBasedGI/VoxelizationDebugRenderer.vert",
	                              "Shaders/VoxelBasedGI/VoxelizationDebugRenderer.frag",
	                              "Shaders/VoxelBasedGI/VoxelizationDebugRenderer.geom",
	                              coneTracingNumOfIncludes, coneTracingIncludes, coneTracingLengths);
	m_firstBounceCompute = Shader("Shaders/VoxelBasedGI/LightBounce/ComputeLightBounce.comp",
	                              coneTracingNumOfIncludes, coneTracingIncludes, coneTracingLengths);

	m_voxelUniformData.voxelDimension = voxelDimensions;
	m_voxelUniformBuffer.UpdateBuffer(m_voxelUniformData);
	m_debugVoxelBatch = std::make_unique<RenderBatch>(std::make_shared<Material>());
	m_debugVoxelBatch->SetShader(m_debugVoxelMaterial);

	glGenBuffers(1, &m_debugVBO);
	glGenVertexArrays(1, &m_debugVAO);
	SwitchVoxelResolution();

	// Done once at startup to ensure consistency
	GenerateRandomUnitSpherePoints(m_coneTraceUniformData.numbers);
	m_coneTraceUniformBuffer.UpdateBuffer(m_coneTraceUniformData);
}

VoxelManager::~VoxelManager()
{
	glDeleteVertexArrays(1, &m_debugVAO);
}

void VoxelManager::ReleaseTextures()
{
	uint64_t texs[3]{m_colorTex3D.second, m_normalTex3D.second, m_colorTex3DFirstBounce.second};
	// Textures bound bindless
	TextureManager::Get()->ReleaseTexturesARB(3, texs);
}

void VoxelManager::ClearTextures()
{
	static float z[] = {0, 0, 0, 0};
	glClearTexImage(m_colorTex3D.first, 0, GL_RGBA, GL_FLOAT, &z);
	glClearTexImage(m_normalTex3D.first, 0, GL_RGBA, GL_FLOAT, &z);
	glClearTexImage(m_colorTex3DFirstBounce.first, 0, GL_RGBA, GL_FLOAT, &z);
}

void VoxelManager::SwitchVoxelResolution()
{
	m_voxelUniformData.m_orthoMat = glm::ortho(0.0f, static_cast<float>(m_voxelUniformData.voxelDimension),
	                                           static_cast<float>(m_voxelUniformData.voxelDimension), 0.0f);

	// Always need a big enough VAO to render the voxel structure
	glBindVertexArray(m_debugVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_debugVBO);
	glBufferData(GL_ARRAY_BUFFER,
	             m_voxelUniformData.voxelDimension * m_voxelUniformData.voxelDimension * m_voxelUniformData.
	             voxelDimension,
	             nullptr, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(uint32_t), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Release previously used textures
	ReleaseTextures();

	// Generate new pair with new res
	m_colorTex3D = TextureManager::Get()->GetTextureHandleResident3D(
		m_voxelUniformData.voxelDimension,
		m_voxelUniformData.voxelDimension,
		m_voxelUniformData.voxelDimension,
		GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_normalTex3D = TextureManager::Get()->GetTextureHandleResident3D(
		m_voxelUniformData.voxelDimension,
		m_voxelUniformData.voxelDimension,
		m_voxelUniformData.voxelDimension,
		GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_colorTex3DFirstBounce = TextureManager::Get()->GetTextureHandleResident3D(m_voxelUniformData.voxelDimension,
		m_voxelUniformData.voxelDimension,
		m_voxelUniformData.voxelDimension,
		GL_RGBA16F, GL_RGBA, GL_FLOAT);

	m_firstBounceCompute.Use();
	m_firstBounceCompute.SetARB("color3DTex", m_colorTex3D.second);
	m_firstBounceCompute.SetARB("normal3DTex", m_normalTex3D.second);
}

uint32_t VoxelManager::GetVoxelTex() const
{
	return m_colorTex3D.first;
}

uint32_t VoxelManager::GetFirstBounceTex() const
{
	return m_colorTex3DFirstBounce.first;
}

uint32_t VoxelManager::GetNormalTex() const
{
	return m_normalTex3D.first;
}

// Small helper variable
static bool shouldClear = true;

void VoxelManager::SetVoxelSceneParams(uint32_t voxelDimensions, float vPerU)
{
	if (voxelDimensions != m_voxelUniformData.voxelDimension)
	{
		m_voxelUniformData.voxelDimension = voxelDimensions;
		m_voxelUniformData.voxelPerUnit = vPerU;
		m_voxelUniformData.voxelSize = 1.0f / vPerU;
		m_voxelUniformBuffer.UpdateBuffer(m_voxelUniformData);
		SwitchVoxelResolution();
		shouldClear = true;
	}
	else if (abs(vPerU - m_voxelUniformData.voxelPerUnit) >
		COMBUSTION_FLOAT_TOLERANCE)
	{
		shouldClear = true;
		m_voxelUniformData.voxelPerUnit = vPerU;
		m_voxelUniformData.voxelSize = 1.0f / vPerU;
		m_voxelUniformBuffer.UpdateBuffer(m_voxelUniformData);
	}
}

void VoxelManager::VoxelizeScene(std::vector<std::shared_ptr<RenderBatch>> sceneBatches, Renderer* renderer)
{
	const auto colorTex3D = m_colorTex3D.first;
	const auto normalTex3D = m_normalTex3D.first;
	// Necessary if size of the to voxelize area changed but not the main resolution of the structure
	if(shouldClear)
	{
		ClearTextures();
		shouldClear = false;
	}
	
	m_voxelizeMaterial.Use();

	// Images not bound bindless
	glBindImageTexture(0, colorTex3D, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	glBindTexture(GL_TEXTURE_3D, normalTex3D);

	glBindImageTexture(1, normalTex3D, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);

	// GL state setup
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, m_voxelUniformData.voxelDimension, m_voxelUniformData.voxelDimension);

	for (int i = 1; i < sceneBatches.size(); ++i)
	{
		auto batch = sceneBatches.at(i).get();
		// Swap the materials to write into our voxel buffer and assign original material again
		const auto mat = batch->GetShader();
		batch->SetShader(m_voxelizeMaterial);
		renderer->Render(batch);
		batch->SetShader(mat);
	}
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glViewport(0, 0, WindowManager::Get()->GetScreenWidth(), WindowManager::Get()->GetScreenHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_3D, colorTex3D);
	glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void VoxelManager::ComputeFirstLightBounce()
{
	const auto firstBounceTex3D = m_colorTex3DFirstBounce.first;
	m_firstBounceCompute.Use();

	glBindImageTexture(2, firstBounceTex3D, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	glDispatchCompute(m_voxelUniformData.voxelDimension, m_voxelUniformData.voxelDimension,
	                  m_voxelUniformData.voxelDimension);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindTexture(GL_TEXTURE_3D, firstBounceTex3D);
	glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void VoxelManager::BindVoxelStructure(bool useFirstBounce)
{
	glBindTexture(GL_TEXTURE_3D, useFirstBounce ? m_colorTex3DFirstBounce.first : m_colorTex3D.first);
}

void VoxelManager::RenderDebugVoxelScene(Renderer* renderer, bool useFirstBounce)
{
	m_debugVoxelMaterial.Use();
	glBindTexture(GL_TEXTURE_3D, useFirstBounce ? m_colorTex3DFirstBounce.first : m_colorTex3D.first);

	renderer->RenderProcedurally(m_voxelUniformData.voxelDimension *
	                             m_voxelUniformData.voxelDimension *
	                             m_voxelUniformData.voxelDimension, m_debugVoxelBatch.get(), m_debugVAO);
}

void VoxelManager::GenerateRandomUnitSpherePoints(glm::vec4 numbers[12]) const
{
	std::random_device dev;
	std::mt19937 generator(dev());
	std::uniform_real_distribution<float> uniform01(0.0f, 1.0f);

	for (int i = 0; i < 12; ++i)
	{
		const float theta = 2.0f * glm::pi<float>() * uniform01(generator);
		const float phi = acos(1.0f - 2.0f * uniform01(generator));
		numbers[i] = glm::vec4{
			sin(phi) * cos(theta),
			sin(phi) + sin(theta),
			cos(phi),
			1
		};
	}
}

void VoxelManager::SetShowGI(bool showGI)
{
	m_coneTraceUniformData.showGI = showGI ? glm::vec4{1} : glm::vec4{0};
	m_coneTraceUniformBuffer.UpdateBuffer(m_coneTraceUniformData);
}

void VoxelManager::SetStepSize(float stepSize)
{
	m_coneTraceUniformData.showGI.y = stepSize;
	m_coneTraceUniformBuffer.UpdateBuffer(m_coneTraceUniformData);
}

void VoxelManager::SetMaxDist(float maxDist)
{
	m_coneTraceUniformData.showGI.z = maxDist;
	m_coneTraceUniformBuffer.UpdateBuffer(m_coneTraceUniformData);
}

void VoxelManager::SetConeAngle(float coneAngle)
{
	m_coneTraceUniformData.showGI.a = coneAngle;
	m_coneTraceUniformBuffer.UpdateBuffer(m_coneTraceUniformData);
}

void VoxelManager::SetShadowSettings(glm::vec4 shadowSettings)
{
	m_coneTraceUniformData.shadowSettings = shadowSettings;
	m_coneTraceUniformBuffer.UpdateBuffer(m_coneTraceUniformData);
}

void VoxelManager::SetSpecularSettings(glm::vec4 specularSettings)
{
	m_coneTraceUniformData.specularSettings = specularSettings;
	m_coneTraceUniformBuffer.UpdateBuffer(m_coneTraceUniformData);
}

void VoxelManager::SetLightingSetting(glm::vec4 lightingSettings)
{
	m_coneTraceUniformData.m_lightingSettings = lightingSettings;
	m_coneTraceUniformBuffer.UpdateBuffer(m_coneTraceUniformData);
}

#include "stdafx.h"
#include "DeferredRenderer.h"

#include <gl/GL.h>

#include "Program.h"
#include "Shapes.h"

DeferredRenderer::DeferredRenderer() : m_screenQuad(ScreenQuad::CreateScreenQuad())
{
	glGenFramebuffers(1, &m_gBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferHandle);
	auto x = WindowManager::Get()->GetScreenWidth();
	auto y = WindowManager::Get()->GetScreenHeight();
	// G-Buffer textures
	const auto pos = TextureManager::Get()->GetFrameBufferTexture(x, y, GL_RGBA16F, GL_RGBA, GL_FLOAT,
	                                                              GL_COLOR_ATTACHMENT0);
	const auto normal = TextureManager::Get()->GetFrameBufferTexture(x, y, GL_RGBA16F, GL_RGBA, GL_FLOAT,
	                                                                 GL_COLOR_ATTACHMENT1);
	const auto emissive = TextureManager::Get()->GetFrameBufferTexture(
		x, y, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT2);
	const auto diffRough = TextureManager::Get()->GetFrameBufferTexture(x, y, GL_RGBA32F, GL_RGBA, GL_FLOAT,
	                                                                    GL_COLOR_ATTACHMENT3);
	// Generate Renderbuffer for deferred renderer
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, x, y);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glDrawBuffers(4, m_attachmentDesc);

	// Material for fullscreen squad
	using namespace VoxelIncludeConstants;
	Shader deferredShader("Shaders/DummyVert.vert", "Shaders/VoxelBasedGI/ConeTracing/ConeTracingDeferred.frag",
	                      nullptr,
	                      coneTracingNumOfIncludes, coneTracingIncludes, coneTracingLengths);
	std::vector<MaterialUtilities::UniformAttribute> uniforms;

	Properties props(glm::vec3(1, 1, 1),
	                 glm::vec3(1, 1, 1),
	                 glm::vec3(1, 1, 1),
	                 0.3f * 32);

	auto tmp = std::make_shared<TextureData>();
	std::vector<MaterialUtilities::ShaderAttribute> tmp2;
	const auto lightingMat =
		std::make_shared<Material>(MaterialUtilities::MaterialPreset::Default,
		                           deferredShader, Program::m_materialIndex++, props, tmp, tmp2);

	const auto screenQuad = std::make_shared<Mesh3D>(ScreenQuad::CreateScreenQuad());

	GeometryManager::Get()->AddGeometry(screenQuad, *lightingMat);

	m_lightingBatch.SetMaterial(lightingMat);
	auto c = DataHolder::Get()->GetCameraShared();
	m_lightingBatch.CollectGeometry();

	// Set textures bindless
	lightingMat->BindShader();
	lightingMat->GetShader().SetARB("positionBuffer", pos);
	lightingMat->GetShader().SetARB("normalBuffer", normal);
	lightingMat->GetShader().SetARB("emissiveBuffer", emissive);
	lightingMat->GetShader().SetARB("diffRoughBuffer", diffRough);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::Render(RenderBatch* batch)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferHandle);

	Renderer::Render(batch);
}

void DeferredRenderer::RenderProcedurally(uint64_t verticesToRender, RenderBatch* batch, uint32_t VAO)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferHandle);

	Renderer::RenderProcedurally(verticesToRender, batch, VAO);
	Assemble();
}

void DeferredRenderer::Assemble()
{
	// Render the G-Buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render fullscreen quad
	m_lightingBatch.BindVAO();
	m_lightingBatch.BindShader();
	m_lightingBatch.BindBatchUniforms();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void DeferredRenderer::Clear()
{
	glClearColor(0, 0, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferHandle);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

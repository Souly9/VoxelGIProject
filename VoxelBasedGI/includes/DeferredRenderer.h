#pragma once
#include "Renderer.h"

class DeferredRenderer : public Renderer
{
public:
	DeferredRenderer();
	void Render(RenderBatch* batch) override;
	// Renders vertices without a vertex buffer
	void RenderProcedurally(uint64_t verticesToRender, RenderBatch* batch, uint32_t VAO) override;

	// Calculates lighting with the current g-buffer
	void Assemble();

	void Clear();
private:
	unsigned int m_gBufferHandle;
	// Currently used color attachments for g-buffer
	unsigned int m_attachmentDesc[4]
		{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	// Members to render fullscreen squad
	RenderBatch m_lightingBatch;
	Mesh3D m_screenQuad;
};

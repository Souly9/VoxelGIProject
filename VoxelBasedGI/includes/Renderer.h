#pragma once
#include "RenderBatch.h"

class Scene;

class Renderer
{
public:
	virtual ~Renderer() = default;
	virtual void Render(RenderBatch* batch);
	virtual void RenderProcedurally(uint64_t verticesToRender, RenderBatch* batch, uint32_t VAO);
protected:
	std::unique_ptr<Scene> m_currentScene;
};

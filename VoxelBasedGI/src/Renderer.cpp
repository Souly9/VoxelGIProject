#include "stdafx.h"
#include "Renderer.h"

void Renderer::Render(RenderBatch* batch)
{
	batch->BindVAO();

	batch->BindShader();
	batch->BindBatchUniforms();
	uint32_t off = 0;

	for (unsigned int i = 0; i < batch->GetNumOfDrawCalls(); ++i)
	{
		batch->BindUniformsForDrawCall(i);

		glDrawElements(GL_TRIANGLES, batch->GetElementsInCall(i), GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * off));
		off += batch->GetElementsInCall(i);
	}
}

void Renderer::RenderProcedurally(uint64_t verticesToRender, RenderBatch* batch, uint32_t VAO)
{
	glBindVertexArray(VAO);
	batch->BindShader();
	glDrawArrays(GL_POINTS, 0, verticesToRender);
}

#include "stdafx.h"
#include <RenderBatch.h>

#include <utility>

bool m_tmp = false;


RenderBatch::RenderBatch(): m_geometryBuffer(std::make_unique<GeometryBatch>())
                            , m_myMaterial()
{
	glGenVertexArrays(1, &m_VAO);
}

RenderBatch::RenderBatch(std::shared_ptr<Material> mat)
	: m_geometryBuffer(std::make_unique<GeometryBatch>())
	  , m_myMaterial(std::move(mat))
{
	glGenVertexArrays(1, &m_VAO);
}

RenderBatch::~RenderBatch()
{
	glDeleteVertexArrays(1, &m_VAO);
}

void RenderBatch::CollectGeometry()
{
	glBindVertexArray(m_VAO);
	m_geometry.clear();
	m_geometry = GeometryManager::Get()->GetGeometryForMaterial(*m_myMaterial);
	m_geometryBuffer->FillBuffer(m_geometry, m_myMaterial->GetShaderAttributes(), m_myMaterial->GetHandles());
	m_drawCallsToIssue = m_geometry.size();
}

void RenderBatch::BindBatchUniforms()
{
	m_myMaterial->BindProperties();
	m_geometryBuffer->BindBuffers();


	if (m_state == TextureUpdate)
	{
		using namespace MaterialUtilities;
		const TextureData* d = m_geometryBuffer->GetTexData();
		const auto textureTags = m_myMaterial->GetUniformAttributes();
		for (const auto attb : textureTags)
		{
			switch (attb)
			{
			case UniformAttribute::ALBEDO:
				{
					m_myMaterial->GetShader().SetARB("opaque", d->m_albedo);
					break;
				}
			case UniformAttribute::NORMALMAP:
				{
					m_myMaterial->GetShader().SetARB("normalMap", d->m_normalMap);
					break;
				}
			case UniformAttribute::DISPLACEMENTMAP:
				{
					m_myMaterial->GetShader().SetARB("displacementMap", d->m_displacement);
					break;
				}
			case UniformAttribute::CUBEMAP:
				{
					m_myMaterial->GetShader().SetARB("skyBox", d->m_normalMap);
					break;
				}
			case UniformAttribute::SPECULARMAP:
				{
					m_myMaterial->GetShader().SetARB("specularMap", d->m_specularMap);
					break;
				}
			}
		}
		m_state = NoUpdate;
	}
}

std::shared_ptr<Material> RenderBatch::SetMaterial(std::shared_ptr<Material> mat)
{
	auto matP = std::move(m_myMaterial);
	m_myMaterial = std::move(mat);
	return matP;
}

std::shared_ptr<Material> RenderBatch::GetMaterial()
{
	return m_myMaterial;
}

void RenderBatch::BindShader()
{
	m_myMaterial->BindShader();
}

void RenderBatch::BindVAO()
{
	glBindVertexArray(m_VAO);
}


void RenderBatch::BindUniformsForDrawCall(uint32_t drawCall)
{
	m_myMaterial->GetShader().SetMat4("modelMat", m_geometry.at(drawCall)->GetModel());
}

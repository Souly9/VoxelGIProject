#include "stdafx.h"
#include "GeometryBatch.h"

using namespace MaterialUtilities;
constexpr int UV_INDEX = 1;
constexpr int NORMAL_INDEX = 2;
constexpr int TANGENT_INDEX = 3;
constexpr int BITANGENT_INDEX = 4;
constexpr int VERTEXCOLOR_INDEX = 5;

GeometryBatch::GeometryBatch()
{
	GenBuffers();
}

GeometryBatch::~GeometryBatch()
{
	uint32_t buffers[2]{m_BufferHandle, m_EBOHandle};
	glDeleteBuffers(2, buffers);
}

void GeometryBatch::Bind()
{
}

void GeometryBatch::Clear()
{
	m_buffer.clear();
	m_indices.clear();
	m_numOfPrimitives.clear();
}

void GeometryBatch::FillBuffer(const std::vector<std::shared_ptr<Mesh3D>>& geometry,
                               const std::vector<ShaderAttribute>& attributes,
                               std::shared_ptr<TextureData> texData)
{
	Clear();
	m_texData = std::move(texData);
	uint64_t bufferElements = 0;
	for (auto mesh : geometry)
	{
		bufferElements += mesh->GetData()->m_positions.size();
		m_numOfPrimitives.push_back(mesh->GetPrimimitveNum());
	}
	m_buffer.reserve(bufferElements * 3);
	for (uint32_t i = 0; i < geometry.size(); ++i)
	{
		AddThreeFloatArray(geometry.at(i)->GetData()->m_positions);
	}
	uint32_t off = 0;
	for (uint32_t i = 0; i < geometry.size(); ++i)
	{
		for (uint32_t a = 0; a < m_numOfPrimitives.at(i); ++a)
		{
			m_indices.push_back(geometry.at(i)->GetData()->m_indices.at(a) +
				off);
		}

		off += m_numOfPrimitives.at(i);
	}


	// Initialize the pointer for the vertex positions
	uint64_t chunkLength = 3, index = 0;
	m_attribPointers.emplace_back(chunkLength, index, index);

	uint64_t offset = bufferElements * chunkLength;
	{
		// Initialize any optional pointers
		for (const auto& attribute : attributes)
		{
			switch (attribute)
			{
			case(ShaderAttribute::TEXCOORDS):
				{
					index = UV_INDEX;

					chunkLength = 2;
					for (uint32_t i = 0; i < geometry.size(); ++i)
					{
						AddTwoFloatArray(geometry.at(i)->GetData()->m_texCoords);
					}
					break;
				}

			case(ShaderAttribute::NORMALS):
				{
					index = NORMAL_INDEX;
					chunkLength = 3;
					for (uint32_t i = 0; i < geometry.size(); ++i)
					{
						AddThreeFloatArray(geometry.at(i)->GetData()->m_normals);
					}
					break;
				}
			case(ShaderAttribute::TANGENTSPACE):
				{
					index = TANGENT_INDEX;
					chunkLength = 3;
					for (uint32_t i = 0; i < geometry.size(); ++i)
					{
						AddThreeFloatArray(geometry.at(i)->GetData()->m_tangents);
					}

					VertexAttribPointer tmp(chunkLength, index, offset);
					offset += bufferElements * chunkLength;
					tmp.BindPointer();
					m_attribPointers.push_back(tmp);

					index = BITANGENT_INDEX;
					for (uint32_t i = 0; i < geometry.size(); ++i)
					{
						AddThreeFloatArray(geometry.at(i)->GetData()->m_biTangents);
					}
					break;
				}
			case(ShaderAttribute::VERTEXCOLOR):
				{
					chunkLength = 3;
					index = VERTEXCOLOR_INDEX;
					for (uint32_t i = 0; i < geometry.size(); ++i)
					{
						AddThreeFloatArray(geometry.at(i)->GetData()->m_colors);
					}
					break;
				}
			}

			const VertexAttribPointer tmp(chunkLength, index, offset);
			offset += bufferElements * chunkLength;
			m_attribPointers.push_back(tmp);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_BufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_buffer.size(), m_buffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

	for (auto& pointer : m_attribPointers)
	{
		pointer.BindPointer();
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint64_t GeometryBatch::GetNumOfPrimitives() const
{
	return m_indices.size();
}

void GeometryBatch::GenBuffers()
{
	glGenBuffers(1, &m_BufferHandle);
	glGenBuffers(1, &m_EBOHandle);
}

uint32_t GeometryBatch::GetOffsetForCall(const uint32_t drawCallIndex)
{
	return m_numOfPrimitives.at(drawCallIndex);
}

void GeometryBatch::BindBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferHandle);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOHandle);
}

TextureData* GeometryBatch::GetTexData() const
{
	return m_texData.get();
}

void GeometryBatch::AddTwoFloatArray(const std::vector<glm::vec2>& floats)
{
	for (const auto& vec : floats)
	{
		m_buffer.push_back(vec.x);
		m_buffer.push_back(vec.y);
	}
}

void GeometryBatch::AddThreeFloatArray(const std::vector<glm::vec3>& floats)
{
	for (const auto& vec : floats)
	{
		m_buffer.push_back(vec.x);
		m_buffer.push_back(vec.y);
		m_buffer.push_back(vec.z);
	}
}

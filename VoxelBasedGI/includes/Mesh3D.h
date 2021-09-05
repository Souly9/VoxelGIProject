#pragma once
#include "Object.h"
#include "VertexStructs.h"

class GeometryBatch;
class InputManager;
class Object;
struct Vertex;

// Helper class to easier access meshes read from .obj
class Mesh3D : public Object
{
public:
	Mesh3D() = default;
	virtual ~Mesh3D() = default;

	Mesh3D(const Mesh3D& mesh);

	void AddPosition(float x, float y, float z) { m_data->m_positions.emplace_back(x, y, z); }
	void AddPosition(glm::vec3 pos) { m_data->m_positions.push_back(pos); }

	void AddNormal(float x, float y, float z) { m_data->m_normals.emplace_back(x, y, z); }
	void AddNormal(glm::vec3 norm) { m_data->m_normals.push_back(norm); }

	void AddTexCoord(float u, float v) { m_data->m_texCoords.emplace_back(u, v); }
	void AddTexCoord(glm::vec2 uv) { m_data->m_texCoords.push_back(uv); }

	void AddVertexColor(float r, float g, float b) { m_data->m_colors.emplace_back(r, g, b); }
	void AddVertexColor(glm::vec3 color) { m_data->m_colors.push_back(color); }

	void AddColorVec(std::vector<glm::vec3>&& colors)
	{
		m_data->m_colors = colors;
	}

	void AddTangents(float x, float y, float z)
	{
		m_data->m_tangents.emplace_back(x, y, z);
	}

	void AddBiTangents(float x, float y, float z)
	{
		m_data->m_biTangents.emplace_back(x, y, z);
	}

	void AddTangentsAndBitangents(glm::vec3 tang, glm::vec3 biTang)
	{
		m_data->m_tangents.push_back(tang);
		m_data->m_biTangents.push_back(biTang);
	}

	void AddIndex(uint32_t index)
	{
		m_data->m_indices.push_back(index);
		m_numPrimitives = m_data->m_indices.size();
	}

	void AddVertex(Vertex&& vert)
	{
		AddPosition(vert.m_position);
		AddTexCoord(vert.m_texCoord);
		AddNormal(vert.m_normal);
		m_data->m_tangents.push_back(vert.m_tangent);
		m_data->m_biTangents.push_back(vert.m_biTangent);
	}

	/*
	 * Poor way to handle this case but since it will never run during rendering performance doesn't matter
	 */
	void AddVertices(std::vector<Vertex> verts)
	{
		for (int i = 0; i < verts.size(); ++i)
		{
			AddVertex(std::move(verts.at(i)));
		}
	}

	GeometryData* GetData() const noexcept { return m_data.get(); }

	/*
	 * Returns the number of vertices to draw, not the same as amount of vertices 
	 */
	uint32_t GetPrimimitveNum() const noexcept { return m_numPrimitives; }
	uint32_t GetVerticesNum() const noexcept { return m_data->m_positions.size(); }

protected:
	bool m_isMatDirty{true};
	std::shared_ptr<GeometryData> m_data{std::make_shared<GeometryData>()};

	uint32_t m_numPrimitives{};
};

#pragma once
#include "stdafx.h"

class SkyboxCube
{
public:
	static Mesh3D GetSkyBoxCube()
	{
		Mesh3D mesh{};
		glm::vec3 skyboxVertices[] =
		{
			// positions          
			glm::vec3{-1.0f, 1.0f, -1.0f},
			glm::vec3{-1.0f, -1.0f, -1.0f},
			glm::vec3{1.0f, -1.0f, -1.0f},
			glm::vec3{1.0f, -1.0f, -1.0f},
			glm::vec3{1.0f, 1.0f, -1.0f},
			glm::vec3{-1.0f, 1.0f, -1.0f},

			glm::vec3{-1.0f, -1.0f, 1.0f},
			glm::vec3{-1.0f, -1.0f, -1.0f},
			glm::vec3{-1.0f, 1.0f, -1.0f},
			glm::vec3{-1.0f, 1.0f, -1.0f},
			glm::vec3{-1.0f, 1.0f, 1.0f},
			glm::vec3{-1.0f, -1.0f, 1.0f},

			glm::vec3{1.0f, -1.0f, -1.0f},
			glm::vec3{1.0f, -1.0f, 1.0f},
			glm::vec3{1.0f, 1.0f, 1.0f},
			glm::vec3{1.0f, 1.0f, 1.0f},
			glm::vec3{1.0f, 1.0f, -1.0f},
			glm::vec3{1.0f, -1.0f, -1.0f},

			glm::vec3{-1.0f, -1.0f, 1.0f},
			glm::vec3{-1.0f, 1.0f, 1.0f},
			glm::vec3{1.0f, 1.0f, 1.0f},
			glm::vec3{1.0f, 1.0f, 1.0f},
			glm::vec3{1.0f, -1.0f, 1.0f},
			glm::vec3{-1.0f, -1.0f, 1.0f},
			glm::vec3{-1.0f, 1.0f, -1.0f},

			glm::vec3{1.0f, 1.0f, -1.0f},
			glm::vec3{1.0f, 1.0f, 1.0f},
			glm::vec3{1.0f, 1.0f, 1.0f},
			glm::vec3{-1.0f, 1.0f, 1.0f},
			glm::vec3{-1.0f, 1.0f, -1.0f},

			glm::vec3{-1.0f, -1.0f, -1.0f},
			glm::vec3{-1.0f, -1.0f, 1.0f},
			glm::vec3{1.0f, -1.0f, -1.0f},
			glm::vec3{1.0f, -1.0f, -1.0f},
			glm::vec3{-1.0f, -1.0f, 1.0f},
			glm::vec3{1.0f, -1.0f, 1.0f}
		};
		for (int i = 0; i < 36; ++i)
		{
			mesh.AddIndex(i);
			mesh.GetData()->m_positions.emplace_back(skyboxVertices[i].x, skyboxVertices[i].y, skyboxVertices[i].z);
		}
		return mesh;
	}
};

class ScreenQuad
{
public:
	static Mesh3D CreateScreenQuad()
	{
		Mesh3D mesh{};
		glm::vec3 skyboxVertices[] =
		{
			// positions          
			glm::vec3{1.0f, 1.0f, 0.0f},
			glm::vec3{1.0f, -1.0f, 0.0f},
			glm::vec3{-1.0f, -1.0f, 0.0f},
			glm::vec3{-1.0f, 1.0f, 0.0f}
		};
		unsigned int ind[] = {
			0, 1, 3,
			1, 2, 3
		};

		for (unsigned int i : ind)
		{
			mesh.AddIndex(i);
		}
		for (auto& skyboxVertice : skyboxVertices)
		{
			mesh.GetData()->m_positions.emplace_back(skyboxVertice.x, skyboxVertice.y, skyboxVertice.z);
		}

		return mesh;
	}
};

class Square
{
public:
	static Mesh3D Create24VertCube()
	{
		Mesh3D mesh{};
		const auto fillPositions = [&mesh]()
		{
			// back
			auto pos = glm::vec3{-1, -1, -1};
			mesh.AddPosition(pos);
			mesh.AddPosition(pos + glm::vec3{0, 2, 0});
			mesh.AddPosition(pos + glm::vec3{2, 2, 0});
			mesh.AddPosition(pos + glm::vec3{2, 0, 0});
			// right
			pos = glm::vec3{1, -1, -1};
			mesh.AddPosition(pos);
			mesh.AddPosition(pos + glm::vec3{0, 2, 0});
			mesh.AddPosition(pos + glm::vec3{0, 2, 2});
			mesh.AddPosition(pos + glm::vec3{0, 0, 2});
			// front
			pos = glm::vec3{1, -1, 1};
			mesh.AddPosition(pos);
			mesh.AddPosition(pos + glm::vec3{0, 2, 0});
			mesh.AddPosition(pos + glm::vec3{-2, 2, 0});
			mesh.AddPosition(pos + glm::vec3{-2, 0, 0});
			// left
			pos = glm::vec3{-1, -1, 1};
			mesh.AddPosition(pos);
			mesh.AddPosition(pos + glm::vec3{0, 2, 0});
			mesh.AddPosition(pos + glm::vec3{0, 2, -2});
			mesh.AddPosition(pos + glm::vec3{0, 0, -2});
			//down
			pos = glm::vec3{-1, -1, 1};
			mesh.AddPosition(pos);
			mesh.AddPosition(pos + glm::vec3{0, 0, -2});
			mesh.AddPosition(pos + glm::vec3{2, 0, -2});
			mesh.AddPosition(pos + glm::vec3{2, 0, 0});
			// top
			pos = glm::vec3{-1, 1, -1};
			mesh.AddPosition(pos);
			mesh.AddPosition(pos + glm::vec3{0, 0, 2});
			mesh.AddPosition(pos + glm::vec3{2, 0, 2});
			mesh.AddPosition(pos + glm::vec3{2, 0, 0});
		};

		const auto fillIndices = [&mesh]()
		{
			for (size_t i = 0; i < 6 * 4; i += 4)
			{
				mesh.AddIndex(i);
				mesh.AddIndex(i + 1);
				mesh.AddIndex(i + 3);
				mesh.AddIndex(i + 3);
				mesh.AddIndex(i + 1);
				mesh.AddIndex(i + 2);
			}
		};
		const auto fillTexCoord = [&mesh]()
		{
			for (size_t i = 0; i < 6 * 4; i += 4)
			{
				mesh.AddTexCoord(glm::vec2{0, 0});
				mesh.AddTexCoord(glm::vec2{1, 0});
				mesh.AddTexCoord(glm::vec2{1, 1});
				mesh.AddTexCoord(glm::vec2{0, 1});
			}
		};

		const auto fillNormals = [&mesh]()
		{
			glm::vec3 ar[]{
				glm::vec3(0, 0, -1),
				glm::vec3(1, 0, 0),
				glm::vec3(0, 0, 1),
				glm::vec3(-1, 0, 0),
				glm::vec3(0, -1, 0),
				glm::vec3(0, 1, 0)
			};
			for (size_t i = 0; i < 6; i++)
			{
				mesh.AddNormal(ar[i]);
				mesh.AddNormal(ar[i]);
				mesh.AddNormal(ar[i]);
				mesh.AddNormal(ar[i]);
			}
		};
		fillPositions();
		fillIndices();
		fillNormals();
		fillTexCoord();
		for (int i = 0; i < mesh.GetData()->m_indices.size(); i += 3)
		{
			auto index = mesh.GetData()->m_indices.at(i);
			auto index2 = mesh.GetData()->m_indices.at(i + 1);
			auto index3 = mesh.GetData()->m_indices.at(i + 2);
		}
		return mesh;
	}
};

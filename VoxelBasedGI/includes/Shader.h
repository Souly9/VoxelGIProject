#ifndef SHADER_H
#define SHADER_H
#include "stdafx.h"
#include <string>
#include <fstream>

#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TextReader.h"

struct SceneUniformBuffer;

class Shader
{
public:
	uint32_t m_ID;

	Shader() = default;

	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr,
	       unsigned int numberOfIncludes = 0,
	       const char* const* paths = nullptr, const int* length = nullptr)
	{
		std::vector<char> vertexCode = TextReader::Get()->ReadFile(vertexPath);
		std::vector<char> fragmentCode = TextReader::Get()->ReadFile(fragmentPath);

		const char* vShaderCode = vertexCode.data();
		const char* fShaderCode = fragmentCode.data();

		// 2. compile shaders
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		//vertex
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		if (numberOfIncludes > 0)
			glCompileShaderIncludeARB(vertex, numberOfIncludes, paths, nullptr);
		else
			glCompileShader(vertex);
		//print errors
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		//Fragment
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		if (numberOfIncludes > 0)
			glCompileShaderIncludeARB(fragment, numberOfIncludes, paths, nullptr);
		else
			glCompileShader(fragment);
		// print compile errors if any
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}


		//shader Pogram
		m_ID = glCreateProgram();

		glAttachShader(m_ID, vertex);
		glAttachShader(m_ID, fragment);
		// Geometry
		if (geometryPath)
			setGeometryShader(geometryPath, numberOfIncludes, paths, nullptr);

		glLinkProgram(m_ID);
		//print errors again
		glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		//free Shader resources
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	Shader(const char* computePath, unsigned int numberOfIncludes = 0,
	       const char* const* paths = nullptr, const int* length = nullptr)
	{
		std::vector<char> computeSource = TextReader::Get()->ReadFile(computePath);
		const char* vShaderCode = computeSource.data();

		// 2. compile shaders
		unsigned int compute;
		int success;
		char infoLog[512];

		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &vShaderCode, nullptr);
		if (numberOfIncludes > 0)
			glCompileShaderIncludeARB(compute, numberOfIncludes, paths, nullptr);
		else
			glCompileShader(compute);
		//print errors
		glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(compute, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		//shader Pogram
		m_ID = glCreateProgram();
		glAttachShader(m_ID, compute);
		glLinkProgram(m_ID);
		//print errors again
		glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		//free Shader resources
		glDeleteShader(compute);
	}

	void setGeometryShader(const char* geometryPath, unsigned int numberOfIncludes = 0,
	                       const char* const* paths = nullptr, const int* length = nullptr)
	{
		std::vector<char> geometryCode = TextReader::Get()->ReadFile(geometryPath);

		const char* gShaderCode = geometryCode.data();

		// 2. compile shaders
		unsigned int geometry;
		int success;
		char infoLog[512];

		//Geometry
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		if (numberOfIncludes > 0)
			glCompileShaderIncludeARB(geometry, numberOfIncludes, paths, nullptr);
		else
			glCompileShader(geometry);
		// print compile errors 
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometry, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		//shader Pogram
		glAttachShader(m_ID, geometry);
		glLinkProgram(m_ID);
		//print errors again
		glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		//free Shader resources
		glDeleteShader(geometry);
	}

	void Use()
	{
		glUseProgram(m_ID);
	}

	void SetMat4(const std::string& name, const glm::mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, value_ptr(value));
	}

	void SetFloat(const std::string& name, const float value) const
	{
		glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
	}

	void SetARB(const std::string& name, const uint64_t value) const
	{
		glUniformHandleui64ARB(glGetUniformLocation(m_ID, name.c_str()), value);
		if (!(glGetError() == GL_NO_ERROR))
		{
			std::cout << glGetError() << '\n';
		}
	}

	void SetVec3(const std::string& name, const glm::vec3 value) const
	{
		glUniform3f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y, value.z);
	}
};


#endif

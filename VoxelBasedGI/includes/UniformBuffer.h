#pragma once
#include "stdafx.h"

template <typename T>
class UniformBuffer
{
public:
	UniformBuffer(uint32_t index) : m_bufferIndex(index), m_bufferHandle{}
	{
		glGenBuffers(1, &m_bufferHandle);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_bufferIndex, m_bufferHandle);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(T), nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	UniformBuffer(uint32_t index, size_t size) : m_bufferIndex(index), m_bufferHandle{}
	{
		glGenBuffers(1, &m_bufferHandle);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_bufferIndex, m_bufferHandle);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	~UniformBuffer() { glDeleteBuffers(1, &m_bufferHandle); }

	void UpdateBuffer(T data)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_bufferIndex, m_bufferHandle);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	template <typename D>
	void UpdateBuffer(size_t offset, std::vector<D> data)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_bufferIndex, m_bufferHandle);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(D) * data.size(), data.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	template <typename D>
	void UpdateBuffer(size_t offset, D data)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_bufferIndex, m_bufferHandle);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(D), &data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void BindBuffer()
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_bufferIndex, m_bufferHandle);
	}

private:
	uint32_t m_bufferIndex;
	uint32_t m_bufferHandle;
};

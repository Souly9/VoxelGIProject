#include "stdafx.h"
#include <TextureReader.h>

static std::unique_ptr<TextureManager> m_pSingleton = std::make_unique<TextureManager>();

TextureManager::TextureManager() :
	m_reader(std::make_unique<TextureReader>())
{
}

TextureManager::~TextureManager()
{
	for (auto& handle : m_paths)
	{
		ReleaseTextureNonARB(m_texHandleMap.at(handle.second));
		ReleaseTextureARB(handle.second);
	}
}

TextureManager* TextureManager::Get()
{
	assert(m_pSingleton);
	return m_pSingleton.get();
}

std::pair<uint32_t, uint64_t> TextureManager::GetTextureHandleResident3D(
	int x, int y, int z, GLenum type, GLenum channelTypes,
	GLenum valueType)
{
	auto tex = m_reader->GenTexture3D(x, y, z, type, channelTypes, valueType);
	uint64_t handle = glGetTextureHandleARB(tex);
	m_texHandleMap[handle] = tex;
	MakeTextureResident(handle);
	return std::pair{tex, handle};
}

uint64_t TextureManager::GetTextureHandle(int x, int y, GLenum type, GLenum channelTypes, GLenum valueType)
{
	auto tex = m_reader->GenTexture(x, y, type, channelTypes, valueType);
	auto handle = glGetTextureHandleARB(tex);
	m_texHandleMap[handle] = tex;
	MakeTextureResident(handle);

	return handle;
}

uint64_t TextureManager::GetFrameBufferTexture(int x, int y, GLenum type, GLenum channelTypes, GLenum valueType,
                                               GLenum target)
{
	auto tex = m_reader->GenTexture(x, y, type, channelTypes, valueType);
	glFramebufferTexture2D(GL_FRAMEBUFFER, target, GL_TEXTURE_2D, tex, 0);
	auto handle = glGetTextureHandleARB(tex);
	m_texHandleMap[handle] = tex;
	MakeTextureResident(handle);

	return handle;
}

uint64_t TextureManager::GetTextureHandle(const char* path, bool bSRGBFormat)
{
	const auto& it = m_paths.find(path);
	if (it != m_paths.end())
		return it->second;

	uint32_t tex = m_reader->GenTexture(path, bSRGBFormat);
	uint64_t handle = glGetTextureHandleARB(tex);
	m_texHandleMap[handle] = tex;
	MakeTextureResident(handle);

	return handle;
}


uint64_t TextureManager::GetCubemapHandle(const char* path, bool bSRGBFormat)
{
	const auto& it = m_paths.find(path);
	if (it != m_paths.end())
		return it->second;

	uint32_t tex = m_reader->GenCubemap(path, bSRGBFormat);
	uint64_t handle = glGetTextureHandleARB(tex);
	m_texHandleMap[handle] = tex;
	MakeTextureResident(handle);
	return handle;
}

void TextureManager::MakeTextureResident(uint64_t textureHandle)
{
	glMakeTextureHandleResidentARB(textureHandle);
}

void TextureManager::ReleaseTexturesARB(uint32_t n, uint64_t* arbTextureHandle)
{
	--n;
	while (n > 0)
	{
		if (arbTextureHandle[n] != 0)
		{
			ReleaseTextureARB(arbTextureHandle[n]);
			ReleaseTextureNonARB(m_texHandleMap.at(arbTextureHandle[n]));
		}
		--n;
	}
}

void TextureManager::ReleaseTexturesNonARB(uint32_t n, uint32_t* textures)
{
	glDeleteTextures(n, textures);
	--n;
	while (n > 0)
	{
		auto bIter = m_texHandleMap.begin();
		for (; bIter != m_texHandleMap.end();)
		{
			if (bIter->second == textures[n])
			{
				ReleaseTextureARB(bIter->first);
				m_texHandleMap.erase(bIter);
				break;
			}
			++bIter;
		}
		--n;
	}
}

void TextureManager::ReleaseTextureARB(uint64_t arbTextureHandle)
{
	glMakeTextureHandleNonResidentARB(arbTextureHandle);
}

void TextureManager::ReleaseTextureNonARB(uint32_t texture)
{
	auto bIter = m_texHandleMap.begin();
	for (; bIter != m_texHandleMap.end();)
	{
		if (bIter->second == texture)
		{
			m_texHandleMap.erase(bIter);
			glDeleteTextures(1, &texture);
			break;
		}
		++bIter;
	}
}

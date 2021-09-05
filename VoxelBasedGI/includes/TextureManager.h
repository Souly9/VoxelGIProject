#pragma once
#include <unordered_map>

class TextureReader;
using GLenum = unsigned int;

class TextureManager
{
public:
	TextureManager();
	~TextureManager();
	static TextureManager* Get();
	std::pair<uint32_t, uint64_t> GetTextureHandleResident3D(int x, int y, int z, GLenum type, GLenum channelTypes,
	                                                         GLenum valueType);
	uint64_t GetTextureHandle(int x, int y, GLenum type, GLenum channelTypes, GLenum valueType);
	uint64_t GetTextureHandle(const char* path, bool bSRGBFormat);
	uint64_t GetCubemapHandle(const char* path, bool bSRGBFormat);
	uint64_t GetFrameBufferTexture(int x, int y, GLenum type, GLenum channelTypes, GLenum valueType, GLenum target);
	void MakeTextureResident(uint64_t textureHandle);
	void ReleaseTextureARB(uint64_t arbTextureHandle);
	void ReleaseTexturesARB(uint32_t n, uint64_t* arbTextureHandle);
	void ReleaseTexturesNonARB(uint32_t n, uint32_t* textures);
	void ReleaseTextureNonARB(uint32_t texture);
private:
	std::unique_ptr<TextureReader> m_reader;
	std::unordered_map<std::string, uint64_t> m_paths;
	std::unordered_map<uint64_t, uint32_t> m_texHandleMap;
};

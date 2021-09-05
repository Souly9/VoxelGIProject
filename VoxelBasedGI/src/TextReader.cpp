#include "TextReader.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
static std::unique_ptr<TextReader> m_pSingleton = std::make_unique<TextReader>();;

TextReader* TextReader::Get()
{
	assert(m_pSingleton);
	return m_pSingleton.get();
}

std::vector<char> TextReader::ReadFile(const char* filePath)
{
	std::fstream fileStream{filePath};
	fileStream.exceptions(std::ifstream::badbit);
	std::vector<char> contents;
	fileStream.seekg(0, std::ios::end);
	const auto len = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);
	contents.resize(len);
	fileStream.read(contents.data(), len);
	return contents;
}

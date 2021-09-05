#pragma once
#include <vector>


class TextReader
{
public:
	static TextReader* Get();

	std::vector<char> ReadFile(const char* filePath);
};

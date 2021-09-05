#include "stdafx.h"
#include "Program.h"

#include "ArcBall.h"

int Program::m_materialIndex;

Program::Program(const char* windowName, int x, int y)
{
	ArcBall::HookEvents();
	WindowManager::Create(x, y, windowName);
}

Program::~Program()
{
	WindowManager::Get()->Close();
}

void Program::AddShaderIncludeDirs(std::vector<std::string> names, std::vector<std::string> directories)
{
	for (int i = 0; i < names.size(); ++i)
	{
		// Every file needs to be read and inserted in virtual file system for shader include
		const auto& dir = directories.at(i);
		std::vector<char> shaderCode = TextReader::Get()->ReadFile(dir.c_str());
		const auto& name = names.at(i);
		glNamedStringARB(GL_SHADER_INCLUDE_ARB, name.size(), name.c_str(), shaderCode.size(), shaderCode.data());
	}
}

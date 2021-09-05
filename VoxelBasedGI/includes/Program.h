#pragma once
#include "DeferredRenderer.h"
#include "Material.h"

class Material;
class Mesh3D;
class RenderBatch;
class HighResClock;
class Camera;
class Scene;
class WindowManager;

class Program
{
public:
	// Only constructs an empty window for us
	Program(const char* windowName, int x, int y);
	virtual ~Program();

	Program(const Program& other) = delete;
	Program* operator=(const Program& other) = delete;

	virtual void Run() = 0;
	void AddShaderIncludeDirs(std::vector<std::string> names, std::vector<std::string> directories);
	static int m_materialIndex;

protected:
	virtual void Update() = 0;
	virtual void Render() = 0;

	// Used to configure GL States for the program
	virtual void SetupGLStates() = 0;

	static void GLAPIENTRY
	MessageCallback(GLenum source,
	                GLenum type,
	                GLuint id,
	                GLenum severity,
	                GLsizei length,
	                const GLchar* message,
	                const void* userParam)
	{
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		        type, severity, message);
	}

	std::unique_ptr<DeferredRenderer> m_pDeferredRenderer;

	std::vector<std::shared_ptr<RenderBatch>> m_renderBatches;
	std::unique_ptr<Scene> m_pMainScene;
};

#include "VoxelBasedGIApp.h"

#include "ArcBall.h"
#include "ModelLoader.h"
#include "Shapes.h"


using namespace MaterialUtilities;

VoxelBasedGIApp::VoxelBasedGIApp(const char* windowName, int x, int y) : Program(windowName, x, y),
                                                           m_guiManager(WindowManager::Get()->GetWindow())
{
	// Need to be added before any shader gets compiled
	AddShaderIncludeDirs(
		{"/ConeTracingFunctions.glsl", "/ConeTracingGlobals.glsl", "/Globals.glsl", "/PBRFunctions.glsl"}, {
			"Shaders/VoxelBasedGI/ConeTracingFunctions.glsl", "Shaders/VoxelBasedGI/ConeTracingGlobals.glsl",
			"Shaders/Globals.glsl", "Shaders/PBR/PBRFunctions.glsl"
		});

	// Set up rotation although no models currently make use of it
	// Uncomment line 167 in ModelLoader to enable
	ArcBall::UpdatePos
	(
		glm::vec2(WindowManager::Get()->GetScreenWidth() / 2.0f, WindowManager::Get()->GetScreenHeight() / 2.0f),
		1.0f *
		static_cast<float>(sqrt
			(
				WindowManager::Get()->GetScreenWidth() * WindowManager::Get()->GetScreenWidth() +
				WindowManager::Get()->GetScreenHeight() * WindowManager::Get()->GetScreenHeight()
			))
	);

	DataHolder::Get()->GetCameraShared()->LookAt(glm::vec4(1.5, 1, 4, 1), glm::vec4(0, 0, 0, 1), glm::vec3(0, 1, 0));

	// Create Skybox material and texture since it needs to be rendered before everything else
	Shader normalShader("Shaders/SkyboxVert.vert", "Shaders/SkyboxFragDeferred.frag");

	std::vector<UniformAttribute> uniforms;
	uniforms.push_back(UniformAttribute::CUBEMAP);

	Properties props(glm::vec3(1, 1, 1),
	                 glm::vec3(1, 1, 1),
	                 glm::vec3(1, 1, 1),
	                 0.3f * 32);

	auto tmp = std::make_shared<TextureData>();
	tmp->m_cubeMap = TextureManager::Get()->GetCubemapHandle(
		"Textures/Yokohama3/", true);
	std::vector<ShaderAttribute> tmp2;
	const auto cubeMap = std::make_shared<Material>(
		MaterialPreset::Default, normalShader, m_materialIndex++, props, tmp, tmp2);

	const auto skyboxCube = std::make_shared<Mesh3D>(SkyboxCube::GetSkyBoxCube());
	GeometryManager::Get()->AddGeometry(skyboxCube, *cubeMap);

	const auto batch = std::make_shared<RenderBatch>(cubeMap);
	batch->CollectGeometry();
	m_renderBatches.push_back(batch);

	// Create all scene data
	m_pMainScene = std::make_unique<Scene>(DataHolder::Get()->GetCameraShared());

	// Voxel GI setup
	m_pVoxelScene = std::make_unique<VoxelManager>(VOXEL_INITIAL_DIMENSIONS);
	m_pVoxelScene->SetVoxelSceneParams(VOXEL_INITIAL_DIMENSIONS, VOXEL_INITIAL_DIMENSIONS / 2.5f);

	// Create the models to render for the scene
	LoadModels(m_guiManager.GetSceneIndex());
	SetupGLStates();

	m_pDeferredRenderer = std::make_unique<DeferredRenderer>();
}

void VoxelBasedGIApp::Run()
{
	while (!glfwWindowShouldClose(WindowManager::Get()->GetWindow()))
	{
		m_pMainScene->BeginScene();

		Update();

		Render();

		m_guiManager.DrawGUI();

		glfwSwapBuffers(WindowManager::Get()->GetWindow());
		glfwPollEvents();
	}
}

void VoxelBasedGIApp::Update()
{
	DataHolder::Get()->Update();

	float delta, d;
	DataHolder::Get()->GetTimes(d, delta);

	assert(delta >= 0);
	SignalSystem::ApplicationEvents::Get()->onUpdate(delta);

	// Handle Voxel GI Update
	assert(m_framesSinceVoxelization >= 0);
	m_framesSinceVoxelization++;

	// Handle GUI Input now
	if (m_guiManager.ShouldChangeScene())
		LoadModels(m_guiManager.GetSceneIndex());

	m_pVoxelScene->SetVoxelSceneParams(m_guiManager.GetVoxelRes(),
	                                   static_cast<float>(m_guiManager.GetVoxelRes()) / m_guiManager.GetVoxelSize());
	m_pVoxelScene->SetShowGI(m_guiManager.GetShowGI());
	m_pVoxelScene->SetStepSize(m_guiManager.GetStepSize());
	m_pVoxelScene->SetMaxDist(m_guiManager.GetMaxDist());
	m_pVoxelScene->SetConeAngle(m_guiManager.GetConeAngle());
	m_pVoxelScene->SetShadowSettings(m_guiManager.GetShadowSettings());
	m_pVoxelScene->SetSpecularSettings(m_guiManager.GetSpecularSettings());
	m_pVoxelScene->SetLightingSetting(m_guiManager.GetLightingSettings());
}

void VoxelBasedGIApp::Render()
{
#ifdef NDEBUG
	assert(m_pDeferredRenderer.get());
	assert(m_pVoxelScene.get());
	assert(m_pMainScene->get());
	assert(m_guiManager);
#endif

	// Updates Renderer and Voxel GI states before rendering skybox and models
	m_pDeferredRenderer->Clear();

	if (m_guiManager.ShouldUpdateGI() &&
		m_guiManager.ShouldComputeSecondLightBounce() &&
		m_framesSinceVoxelization == std::max(m_guiManager.GetFramesBetweenVoxelization() / 2, 1))
	{
		m_pVoxelScene->ComputeFirstLightBounce();
	}

	if (m_guiManager.ShouldUpdateGI() &&
		m_framesSinceVoxelization >= m_guiManager.GetFramesBetweenVoxelization())
	{
		
		m_pVoxelScene->VoxelizeScene(m_renderBatches, m_pDeferredRenderer.get());
		m_framesSinceVoxelization = 0;
	}

	// Skybox can't have depth test enabled
	glDisable(GL_DEPTH_TEST);
	m_pDeferredRenderer->Render(m_renderBatches.at(0).get());
	glEnable(GL_DEPTH_TEST);

	// Iterate over all models and render them
	for (auto i = 1; i < m_renderBatches.size(); ++i)
	{
		m_pVoxelScene->BindVoxelStructure(m_guiManager.ShouldComputeSecondLightBounce());
		// Nothing to render if we're rendering the voxel structure
		if (!m_guiManager.ShowDebug())
		{
			m_pDeferredRenderer->Render(m_renderBatches.at(i).get());
		}
	}
	// Render voxel structure
	if (m_guiManager.ShowDebug())
	{
		m_pVoxelScene->RenderDebugVoxelScene(m_pDeferredRenderer.get(), m_guiManager.ShouldComputeSecondLightBounce());
	}

	// Creates final frame
	m_pDeferredRenderer->Assemble();
}

void VoxelBasedGIApp::SetupGLStates()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// During init, enable debug output
#ifdef NDEBUG
    glDisable(GL_DEBUG_OUTPUT);
#else
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, nullptr);
#endif
	// Not really needed here
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CONSERVATIVE_RASTERIZATION_NV);
}

void VoxelBasedGIApp::LoadModels(int scene)
{
#ifdef NDEBUG
	assert(m_pVoxelScene.get());
	assert(m_renderBatches);
	assert(m_pMainScene->get());
#endif
	
	ModelLoader loader;
	// Don't eras skybox batch
	m_renderBatches.erase(m_renderBatches.begin() + 1, m_renderBatches.end());
	// Previously voxelized scene isn't valid anymore
	m_pVoxelScene->ClearTextures();
	// Every scene has its own light
	m_pMainScene->GetLightManager()->Clear();

	if (scene == 0)
	{
		PointLight light{
			glm::vec4(1.1, 1.95, 0.95, 1),
			glm::vec4(23.47, 21.31, 20.79, 1)
		};

		m_pMainScene->GetLightManager()->AddLight(light);
		loader.LoadModel("Models/CornellBox/CornellBox-Original.obj", m_renderBatches);
	}
	if (scene == 1)
	{
		PointLight light{
			glm::vec4(1.1, 1.95, 0.95, 1),
			glm::vec4(23.47, 21.31, 20.79, 1)
		};

		m_pMainScene->GetLightManager()->AddLight(light);

		loader.LoadModel("Models/CornellBox/CornellBox-Empty-RG.obj", m_renderBatches);
		loader.LoadModel("Models/bunny.obj", m_renderBatches);
		auto mesh = GeometryManager::Get()->GetGeometryForMaterial(
			*m_renderBatches.at(m_renderBatches.size() - 1)->GetMaterial());
		mesh.at(0)->Scale(9);
		mesh.at(0)->AdditiveTranslation(glm::vec3{0.35f, 0, 0});
	}
	if (scene == 2)
	{
		PointLight light{
			glm::vec4(1.1, 1.5, 0.95, 1),
			glm::vec4(23.47, 21.31, 20.79, 1)
		};

		m_pMainScene->GetLightManager()->AddLight(light);

		loader.LoadModel("Models/CornellBox/CornellBox-Sphere.obj", m_renderBatches);
	}
	if (scene == 3)
	{
		PointLight light{
			glm::vec4(1.1, 1.5, 0.95, 1),
			glm::vec4(23.47, 21.31, 20.79, 1)
		};

		m_pMainScene->GetLightManager()->AddLight(light);

		loader.LoadModel("Models/CornellBox/CornellBox-Sphere.obj", m_renderBatches);
		loader.LoadModel("Models/lucy.obj", m_renderBatches);
		auto mesh = GeometryManager::Get()->GetGeometryForMaterial(
			*m_renderBatches.at(m_renderBatches.size() - 1)->GetMaterial());
		mesh.at(0)->AdditiveTranslation(glm::vec3{0, 0, -0.2});
	}
	if (scene == 4)
	{
		PointLight light{
			glm::vec4(1.1, 1.95, 0.95, 1),
			glm::vec4(23.47, 21.31, 20.79, 1)
		};

		m_pMainScene->GetLightManager()->AddLight(light);

		loader.LoadModel("Models/CornellBox/CornellBox-Empty-CO.obj", m_renderBatches);
		loader.LoadModel("Models/lucy.obj", m_renderBatches);
		auto mesh = GeometryManager::Get()->GetGeometryForMaterial(
			*m_renderBatches.at(m_renderBatches.size() - 1)->GetMaterial());
		mesh.at(0)->Scale(2);
		mesh.at(0)->AdditiveTranslation(glm::vec3{0, 0, -0.2});
	}
	if (scene == 5)
	{
		PointLight light{
			glm::vec4(1.1, 1.95, 0.95, 1),
			glm::vec4(23.47, 21.31, 20.79, 1)
		};

		m_pMainScene->GetLightManager()->AddLight(light);

		loader.LoadModel("Models/CornellBox/CornellBox-Water.obj", m_renderBatches);
	}
	if (scene == 6)
	{
		PointLight light{
			glm::vec4(1.1, 1.95, 0.95, 1),
			glm::vec4(23.47, 21.31, 20.79, 1)
		};
		PointLight light2{
			glm::vec4(1.1, 1, 0.95, 1),
			glm::vec4(23.47, 0, 0, 1)
		};

		m_pMainScene->GetLightManager()->AddLight(light);
		m_pMainScene->GetLightManager()->AddLight(light2);

		loader.LoadModel("Models/CornellBox/CornellBox-Water.obj", m_renderBatches);
		loader.LoadModel("Models/lucy.obj", m_renderBatches);
		auto mesh = GeometryManager::Get()->GetGeometryForMaterial(
			*m_renderBatches.at(m_renderBatches.size() - 1)->GetMaterial());
		mesh.at(0)->AdditiveTranslation(glm::vec3{0, 0, -0.2});
	}
}

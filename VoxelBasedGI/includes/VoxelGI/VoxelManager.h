#pragma once

class Renderer;

class VoxelManager
{
public:
	// Sets up the initial parameters and initial variables for given voxel dimensions
	VoxelManager(uint32_t voxelDimensions);
	~VoxelManager();

	// Generates all textures for current voxel dimensions
	void SwitchVoxelResolution();

	/*
	 * Voxelices a given scene given by its RenderBatches (Due to the program not having more scenes)
	 * @param sceneBatches All the geometry of the scene to voxelize
	 * @param renderer The renderer with which to run the voxelization rendering pass
	 */
	void VoxelizeScene(std::vector<std::shared_ptr<RenderBatch>> sceneBatches, Renderer* renderer);
	// Computes the first indirect light bounce by dispatching compute shader
	void ComputeFirstLightBounce();
	/*
	 * Renders the visualization of the voxel structure
	 * @param renderer The renderer to use
	 * @param useFirstBounce Whether to use the texture with zero or one indirect light bounce
	 */
	void RenderDebugVoxelScene(Renderer* renderer, bool useFirstBounce);

	// Generates 12 random points on the unit sphere according to http://corysimon.github.io/articles/uniformdistn-on-sphere/
	void GenerateRandomUnitSpherePoints(glm::vec4 numbers[12]) const;
	/*
	 * -----------------------------------------------------------------------
	 * All setters necessary to handle GUI input
	 * -----------------------------------------------------------------------
	 */
	void SetVoxelSceneParams(uint32_t voxelDimensions, float vPerU);
	void SetShowGI(bool showGI);
	void SetStepSize(float stepSize);
	void SetMaxDist(float maxDist);
	void SetConeAngle(float coneAngle);
	void SetShadowSettings(glm::vec4 shadowSettings);
	void SetSpecularSettings(glm::vec4 specularSettings);
	void SetLightingSetting(glm::vec4 lightingSettings);

	// Bind the texture
	void BindVoxelStructure(bool useFirstBounce);
	// Clear all voxel textures
	void ClearTextures();

	/*
	 * -----------------------------------------------------------------------
	 * Getters
	 * -----------------------------------------------------------------------
	 */
	uint32_t GetVoxelTex() const;
	uint32_t GetFirstBounceTex() const;
	uint32_t GetNormalTex() const;
protected:
	// Cleanup method
	void ReleaseTextures();

	/*
	 * -----------------------------------------------------------------------
	 * Uniform Buffer structs for use in shaders
	 * -----------------------------------------------------------------------
	 */
	struct VoxelSceneUniformBuffer
	{
	public:
		glm::mat4 m_orthoMat;
		float voxelPerUnit;
		uint32_t voxelDimension;
		float voxelSize;
	};

	struct ConeTraceBuffer
	{
	public:
		glm::vec4 numbers[12];
		glm::vec4 showGI;
		glm::vec4 shadowSettings;
		glm::vec4 specularSettings;
		glm::vec4 m_lightingSettings;
	};

	/*
	 * -----------------------------------------------------------------------
	 * Uniform Buffer classes and data
	 * -----------------------------------------------------------------------
	 */
	VoxelSceneUniformBuffer m_voxelUniformData;
	UniformBuffer<VoxelSceneUniformBuffer> m_voxelUniformBuffer;
	ConeTraceBuffer m_coneTraceUniformData;
	UniformBuffer<ConeTraceBuffer> m_coneTraceUniformBuffer;

	std::unique_ptr<RenderBatch> m_debugVoxelBatch;

	std::pair<uint32_t, uint64_t> m_colorTex3D{};
	std::pair<uint32_t, uint64_t> m_normalTex3D{};
	std::pair<uint32_t, uint64_t> m_colorTex3DFirstBounce{};

	uint32_t m_debugVBO{};
	uint32_t m_debugVAO{};

	Shader m_voxelizeMaterial;
	Shader m_debugVoxelMaterial;
	Shader m_firstBounceCompute;
};

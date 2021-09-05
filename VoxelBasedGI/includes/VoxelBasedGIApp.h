#pragma once
#include "Program.h"
#include "VoxelGI/VoxelManager.h"
#include "VoxelBasedGUI.h"

class VoxelManager;
class VoxelBasedGUI;

class VoxelBasedGIApp : public Program
{
public:
	// Sets up all states
	VoxelBasedGIApp(const char* windowName, int x, int y);
	void Run() override;

protected:
	void Update() override;
	void Render() override;
	void SetupGLStates() override;
	/*
	 * Loads the needed models for the pre-defined scene based on user input
	 * @param scene The index of the scene
	 */
	void LoadModels(int scene);

	// Manages the Voxel Based GI algorithm alone
	std::unique_ptr<VoxelManager> m_pVoxelScene;

	// Receives input
	VoxelBasedGUI m_guiManager;

	// Needed to determine when to re-voxelize
	unsigned int m_framesSinceVoxelization{};
};

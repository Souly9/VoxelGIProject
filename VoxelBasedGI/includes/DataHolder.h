#pragma once

// Simple class to manage the central camera and current time
class DataHolder
{
public:
	DataHolder();

	static DataHolder* Get();

	void GetTimes(float& totalTime, float& deltaTime) const;
	std::shared_ptr<Camera> GetCameraShared() const;
	Camera* GetCameraRaw() const;

	void Update();

private:
	std::unique_ptr<HighResClock> m_pClock;
	std::shared_ptr<Camera> m_pMainCamera;
};

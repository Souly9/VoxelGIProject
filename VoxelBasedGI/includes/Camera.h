#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera(float fov, float zNear, float zFar, float aspectRatio, glm::vec4 pos, glm::quat rot);
	~Camera();

	void UpdateMatrices();
	void LookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 upDirection);

	/*
	 * -----------------------------------------------------------------------
	 * Getters/Setters for camera values
	 * -----------------------------------------------------------------------
	 */
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	glm::mat4 GetViewProjectionMatrix() const;
	void set_FoV(float fov);
	void SetPositions(glm::vec4 position);
	glm::vec3 GetPosition() const;
	void SetRotation(const glm::quat& rotation);
	glm::quat GetRotation() const;

	/*
	 * -----------------------------------------------------------------------
	 * Methods handling mouse and keyboard input for camera movement
	 * -----------------------------------------------------------------------
	 */
	void Move(const glm::vec4& input);
	void LookAround(const glm::vec2& input);
	void Translate(const glm::vec4& trans);
	void Scroll(float val);

private:
	__declspec(align(16)) struct AlignedCameraData
	{
		glm::vec4 m_position;
		glm::vec4 m_target;
		glm::vec4 m_upDirection;
		glm::vec4 m_frontDirection;
		glm::quat m_rotationQuat;

		glm::mat4 m_viewMatrix;
		glm::mat4 m_projMatrix;

		glm::mat4 m_viewProjMatrix;
	};

	std::unique_ptr<AlignedCameraData> m_pData;
	glm::vec2 m_lastMousePos;
	glm::vec2 m_yawPitch{-90, 0};
	MouseRotation m_rotatorState;

	float m_FoV;
	float m_zNear;
	float m_zFar;
	float m_AspectRatio;
	float m_lastDeltaTime;

	bool m_isDirty;
};

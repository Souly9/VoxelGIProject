#include "stdafx.h"
#include "Camera.h"

Camera::Camera(float fov, float zNear, float zFar, float aspectRatio,
               glm::vec4 pos,
               glm::quat rot)
	: m_FoV(fov),
	  m_zNear(zNear),
	  m_zFar(zFar),
	  m_AspectRatio(aspectRatio)
{
	m_rotatorState = ShouldIgnore;
	SignalSystem::InputSignals::Get()->onMouseScroll.connect([this](const auto& val) { Scroll(val); });
	SignalSystem::ApplicationEvents::Get()->onScreenResize.connect([this](const glm::vec2& size)
	{
		m_AspectRatio = size.x / size.y;
	});
	SignalSystem::InputSignals::Get()->onWASDPress.connect([this](const auto& vec) { Move(vec); });
	SignalSystem::InputSignals::Get()->onMouseLeftDrag.connect([this](const glm::vec2& vec) { LookAround(vec); });
	SignalSystem::InputSignals::Get()->onMouseStartLeftDrag.connect([this](const glm::vec2& vec)
	{
		m_rotatorState = ShouldRespond;
		m_lastMousePos = vec;
	});
	SignalSystem::InputSignals::Get()->onMouseStopLeftDrag.connect([this](const auto& vec)
	{
		m_rotatorState = ShouldIgnore;
	});
	SignalSystem::ApplicationEvents::Get()->onUpdate.connect([this](const float deltaTime)
	{
		m_lastDeltaTime = deltaTime;
	});
	const auto p = static_cast<AlignedCameraData*>(_aligned_malloc(sizeof(AlignedCameraData), 16));
	m_pData = std::unique_ptr<AlignedCameraData>(p);
	m_pData->m_target = glm::vec4(0, 0, 0, 1);
	m_pData->m_upDirection = glm::vec4(0, 1, 0, 0);
	m_pData->m_position = pos;
	m_pData->m_rotationQuat = rot;
	m_pData->m_frontDirection = glm::vec4(0, 0, -1, 0);
}

Camera::~Camera()
{
	// Take out the object
	auto p = m_pData.release();
	// Delete it manually
	_aligned_free(p);
}

void Camera::UpdateMatrices()
{
	if (m_isDirty)
	{
		SignalSystem::ApplicationEvents::Get()->onMainCameraValsChanged();
		m_pData->m_projMatrix = glm::perspective(glm::radians(m_FoV), m_AspectRatio, m_zNear, m_zFar);
		LookAt(m_pData->m_position,
		       m_pData->m_position + m_pData->m_frontDirection,
		       m_pData->m_upDirection);
		m_pData->m_viewProjMatrix = m_pData->m_projMatrix * m_pData->m_viewMatrix;
		m_isDirty = false;
	}
}

void Camera::LookAt(const glm::vec3 pos, const glm::vec3 target, const glm::vec3 upDirection)
{
	using namespace glm;
	m_pData->m_position = vec4(pos.x, pos.y, pos.z, 1);
	m_pData->m_target = vec4(target.x, target.y, target.z, 1);
	m_pData->m_upDirection = vec4(upDirection.x, upDirection.y, upDirection.z, 0);
	const vec3 zAxis = normalize(pos - target);
	const vec3 xAxis = normalize(
		cross(normalize(upDirection), zAxis));
	const vec3 yAxis = cross(zAxis, xAxis);


	m_pData->m_viewMatrix = mat4{
		xAxis.x, yAxis.x, zAxis.x, 0,
		xAxis.y, yAxis.y, zAxis.y, 0,
		xAxis.z, yAxis.z, zAxis.z, 0,
		-dot(xAxis, pos), -dot(yAxis, pos), -dot(zAxis, pos), 1
	};
	m_isDirty = true;
}

glm::mat4 Camera::GetViewMatrix() const
{
	return m_pData->m_viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
	return m_pData->m_projMatrix;
}

glm::mat4 Camera::GetViewProjectionMatrix() const
{
	return m_pData->m_viewProjMatrix;
}

void Camera::set_FoV(const float fov)
{
	m_FoV = fov;
	m_isDirty = true;
}

void Camera::SetPositions(const glm::vec4 position)
{
	m_pData->m_position = position;
}

glm::vec3 Camera::GetPosition() const
{
	return m_pData->m_position;
}

void Camera::SetRotation(const glm::quat& rotation)
{
	m_pData->m_rotationQuat = rotation;
	m_isDirty = true;
}

glm::quat Camera::GetRotation() const
{
	return m_pData->m_rotationQuat;
}

void Camera::Move(const glm::vec4& input)
{
	// Scaling
	float delta = m_lastDeltaTime * 30;
	Translate(input.y * m_pData->m_frontDirection * delta);
	Translate(input.z * glm::vec4(normalize(cross(
		                              glm::vec3{
			                              m_pData->m_frontDirection.x, m_pData->m_frontDirection.y,
			                              m_pData->m_frontDirection.z
		                              },
		                              glm::vec3{
			                              m_pData->m_upDirection.x, m_pData->m_upDirection.y, m_pData->m_upDirection.z
		                              })), 1) * delta);
}

void Camera::LookAround(const glm::vec2& input)
{
	if (m_rotatorState == ShouldRespond)
	{
		const float xOff = (input.x - m_lastMousePos.x) * 0.1f;
		const float yOff = (input.y - m_lastMousePos.y) * 0.1f;

		m_lastMousePos = input;

		m_yawPitch.x += xOff;
		m_yawPitch.y += yOff;

		if (m_yawPitch.y > 89.0f)
			m_yawPitch.y = 89;
		if (m_yawPitch.y < -89.0)
			m_yawPitch.y = -89;

		glm::vec4 direction;
		direction.x = cos(glm::radians(m_yawPitch.x)) * cos(glm::radians(m_yawPitch.y));
		direction.y = sin(glm::radians(m_yawPitch.y));
		direction.z = sin(glm::radians(m_yawPitch.x)) * cos(glm::radians(m_yawPitch.y));
		direction.w = 0;
		m_pData->m_frontDirection = normalize(direction);
		m_isDirty = true;
	}
}

void Camera::Translate(const glm::vec4& trans)
{
	m_pData->m_position += trans;
	m_isDirty = true;
}

void Camera::Scroll(const float val)
{
	const glm::mat4& view = m_pData->m_viewMatrix;
	const glm::vec4 dir
	{
		view[0][2],
		view[1][2],
		view[2][2],
		1
	};
	Translate(dir * (val * 0.1f));
}

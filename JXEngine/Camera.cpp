#include "Camera.h"
#include "Config.h"
#include "Material.h"
#include "ShaderCompiler.h"
#include <iostream>

glm::mat4 Camera::GetProjectionMatrix() const
{
	if(projectionType == ProjectionType::Perspective)
		return glm::perspective(glm::radians(Zoom), CONFIG::CAMERA_CONFIG::ASPECT, nearPlane, farPlane);
	return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	//std::cout << "xxx" << std::endl;
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
	//std::cout << Position.x << ' ' << Position.y << ' ' << Position.z  << ' ' << deltaTime << std::endl;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;

		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	Zoom -= (float)yoffset;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
}

void Camera::LoadInfoToShader(std::shared_ptr<Material> mat)
{
	mat->Active();
	mat->SetViewPos(*this);
	mat->GetShader()->SetVec3(CONFIG::CAMERA_CONFIG::SHADER::POSITION_LAST_FRAME, LastPosition);
	mat->GetShader()->SetVec3(CONFIG::CAMERA_CONFIG::SHADER::CENTER_DIR, Front);
	mat->GetShader()->SetVec3(CONFIG::CAMERA_CONFIG::SHADER::CENTER_DIR_LAST_FRAME, LastFront);
	mat->GetShader()->SetFloat(CONFIG::CAMERA_CONFIG::SHADER::FAR_PLANE, farPlane);
	mat->GetShader()->SetFloat(CONFIG::CAMERA_CONFIG::SHADER::NEAR_PLANE, nearPlane);
	//mat->GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX_PRE, this->Pre_ProjMat);
	//mat->GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX_PRE, this->Pre_ViewMat);
}

void Camera::UpdatePreMat()
{
	this->Pre_ViewMat = GetViewMatrix();
	this->Pre_ProjMat = GetProjectionMatrix();
}

void Camera::UpdatePreAttr()
{
	this->LastPosition = Position;
	this->LastFront = Front;
}

void Camera::updateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}

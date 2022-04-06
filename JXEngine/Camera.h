#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>
#include "Config.h"

class Material;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Camera projection type.
// 1.Ortho.
// 2.Perspective.
// ...
enum class ProjectionType
{
	Ortho,
	Perspective
};

// Default camera values
const float YAW = 90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// last camera Attributes
	glm::vec3 LastPosition;
	glm::vec3 LastFront;

	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// Near plane.
	// ...
	float nearPlane;
	
	// Far plane.
	// ...
	float farPlane;

	// ...
	ProjectionType projectionType;

	// Camera matrix of the previous frame.
	// ...
	glm::mat4 Pre_ViewMat;

	// Cropping matrix of the previous frame.
	// ...
	glm::mat4 Pre_ProjMat;

	// constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, 1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		LastPosition = Position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		projectionType = ProjectionType::Perspective;
		
		nearPlane = CONFIG::CAMERA_CONFIG::NEAR_PLANE;
		farPlane = CONFIG::CAMERA_CONFIG::FAR_PLANE;
		updateCameraVectors();
		LastFront = Front;
	}

	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		LastPosition = Position;
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		projectionType = ProjectionType::Perspective;
		nearPlane = CONFIG::CAMERA_CONFIG::NEAR_PLANE;
		farPlane = CONFIG::CAMERA_CONFIG::FAR_PLANE;
		updateCameraVectors();
		LastFront = Front;
	}

	Camera& operator=(const Camera& c)
	{
		this->farPlane = c.farPlane;
		this->Front = c.Front;
		this->LastFront = c.LastFront;
		this->MouseSensitivity = c.MouseSensitivity;
		this->MovementSpeed = c.MovementSpeed;
		this->nearPlane = c.nearPlane;
		this->Pitch = c.Pitch;
		this->Position = c.Position;
		this->LastPosition = c.LastPosition;
		this->projectionType = c.projectionType;
		this->Right = c.Right;
		this->Up = c.Up;
		this->WorldUp = c.WorldUp;
		this->Yaw = c.Yaw;
		this->Zoom = c.Zoom;

		return *this;
	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	glm::mat4 GetProjectionMatrix() const;

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset);

	// Load some camera info to shader, e.g. viewPos, farPlane, nearPlane etc.
	// ...
	void LoadInfoToShader(std::shared_ptr<Material> mat);

	// Update view, projection mat4.
	// ...
	void UpdatePreMat();

	// Update the lastXX attributes.
	// ...
	void UpdatePreAttr();

private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors();
};
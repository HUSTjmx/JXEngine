#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Camera.h"

namespace INPUT
{
	extern std::shared_ptr<Camera> inputCamera;

	extern bool firstMouse;
	extern float lastX;
	extern float lastY;
	
	extern void SET_CAMERA(std::shared_ptr<Camera> c);

	extern void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	extern void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	extern void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	extern void processInput(GLFWwindow* window);

}


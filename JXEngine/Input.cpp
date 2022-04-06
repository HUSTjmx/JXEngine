#include "Timer.h"
#include "Config.h"
#include "Input.h"

namespace INPUT
{
	std::shared_ptr<Camera> inputCamera;

	bool firstMouse = true;
	float lastX = CONFIG::SCREEN_CONFIG::SCR_WIDTH / 2.0f;
	float lastY = CONFIG::SCREEN_CONFIG::SCR_WIDTH / 2.0f;

	void SET_CAMERA(std::shared_ptr<Camera> c)
	{
		inputCamera = c;
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window);

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}

	void processInput(GLFWwindow* window)
	{

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			inputCamera->ProcessKeyboard(FORWARD, Clock.DeltaTime());
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			inputCamera->ProcessKeyboard(BACKWARD, Clock.DeltaTime());
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			inputCamera->ProcessKeyboard(LEFT, Clock.DeltaTime());
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			inputCamera->ProcessKeyboard(RIGHT, Clock.DeltaTime());
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			inputCamera->MovementSpeed += 0.5;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			inputCamera->MovementSpeed -= 0.5;
	}

	// glfw: whenever the mouse moves, this callback is called
	// -------------------------------------------------------
	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		inputCamera->ProcessMouseMovement(xoffset, yoffset);
	}

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		inputCamera->ProcessMouseScroll(yoffset);
	}
}


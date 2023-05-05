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
		//std::cout << "xxx" << std::endl;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			inputCamera->ProcessKeyboard(FORWARD, Clock.DeltaTime());
			//std::cout << "xxx" << std::endl;
		}
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

	int addWin = 0;
	int sceneID = 0;
	int objID = 0;
	int method_ID = 0;
	bool useGauss = true;
	int changeID = 0;
	std::string exePath = "sonExe";
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_F && action == GLFW_PRESS)
		{
			sceneID++;
			sceneID = sceneID % 4;
		}

		if (key == GLFW_KEY_G && action == GLFW_PRESS)
		{
			method_ID++;
			method_ID = method_ID % 4;
			if (sceneID == 0)method_ID = 0;
		}

		if (key == GLFW_KEY_H && action == GLFW_PRESS)
		{
			if (sceneID != 1)objID = 0;
			else
			{
				objID++;
				objID = objID % 3;
			}
		}

		std::string exePath2 = exePath + "\\JXEngine_" + std::to_string(sceneID) + '_' + std::to_string(method_ID) + '_' + std::to_string(objID) + ".exe";


		if (key == GLFW_KEY_J && action == GLFW_PRESS)
		{
			useGauss = !useGauss;
		}

		if (key == GLFW_KEY_K && action == GLFW_PRESS)
		{
			changeID++;
			changeID = changeID % 3;
		}

		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
			if(!useGauss)exePath2 = exePath + "\\JXEngine_2_2_0_NGauss.exe";
			if (changeID == 1)exePath2 = exePath + "\\Change\\JXEngine_1_0_1_Change.exe";
			if (changeID == 2)exePath2 = exePath + "\\Change\\JXEngine_1_0_2_Change.exe";
			if (changeID == 3)exePath2 = exePath + "\\Change\\JXEngine_2_0_0_Change.exe";
			//std::cout << exePath2 << std::endl;
			system(exePath2.c_str());
		}
	}
}


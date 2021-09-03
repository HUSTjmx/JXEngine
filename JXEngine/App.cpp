#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

#include "EmbeddedData.h"
#include "StingShaderTest.h"
#include "Texture.h"
#include "Tools.h"
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "Input.h"
#include "Config.h"
#include "TestScene.h"


void InitGlfw();
void TestAndSetWindow(GLFWwindow* window);
void GetSupportExtensions();
bool CheckExtension(const std::string& extensionName);

void LoadOpenGLFuncPoint();
void Loop(GLFWwindow*);

static std::vector<std::string> g_supportExtensions;

int main()
{
	//初始化Glfw，并设置
	InitGlfw();

	//Window window(SCR_WIDTH, SCR_HEIGHT);
	
	GLFWwindow* window = glfwCreateWindow(CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

	std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

	INPUT::SET_CAMERA(camera);

	TestAndSetWindow(window);

	LoadOpenGLFuncPoint();

	GetSupportExtensions();

	if (!CheckExtension("GL_ARB_shading_language_include"))
	{
		std::cout << "GPU doesn't support GL_ARB_shading_language_include" << std::endl;
	}

	Loop(window);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void InitGlfw()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void TestAndSetWindow(GLFWwindow* window)
{
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, INPUT::framebuffer_size_callback);
	glfwSetCursorPosCallback(window, INPUT::mouse_callback);
	glfwSetScrollCallback(window, INPUT::scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void GetSupportExtensions()
{
	if (!g_supportExtensions.empty())
		return;

	GLint n, i;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for (i = 0; i < n; i++)
	{
		std::string extension = (char*)glGetStringi(GL_EXTENSIONS, i);
		//CEPHEI_LOGDEBUGF("%s ", extension.c_str());
		g_supportExtensions.push_back(extension);
	}
}

bool CheckExtension(const std::string& extensionName)
{
	for (int i = 0; i < g_supportExtensions.size(); i++)
	{
		if (g_supportExtensions[i] == extensionName)
			return true;
	}

	return false;
}

void LoadOpenGLFuncPoint()
{
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return ;
	}
}

void Loop(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);


	Scene scene = OPENGL_SCENE::TestScene::Instance().GetScene_AlphaBlend_03();

	//时钟重置，开始计时
	Clock.Reset();

	//scene.GetActorByIndex(0)->AnimaTo(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 2.0, 1.0), 30.0f);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		Clock.Timing();

		// input
		// -----
		INPUT::processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


		scene.Draw();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	scene.Delete();

}
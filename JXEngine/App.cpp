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
#include "Pass.h"
#include "FrameBuffer.h"
#include "Actor.h"
#include "Material.h"


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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	Pass p = OPENGL_SCENE::TestPass::Intance().GetPass_UniformBufferTest_06();


	

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//OPENGL_SCENE::TestPass::Intance().Draw_FrameTest_04(p1, p2);
		p.Draw();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


}


//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <stb_image.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "learnopengl/shader_m.h"
//#include "learnopengl/camera.h"
////#include "learnopengl/model.h"
//
//#include "ShaderCompiler.h"
//#include "UniformBuffer.h"
//#include "Config.h"
//#include "VertexModel.h"
//#include "EmbeddedData.h"
//#include "Material.h"
//#include "Actor.h"
//#include <iostream>
//
//using VertexModelPtr = std::shared_ptr<VertexModel>;
//using MaterialPtr = std::shared_ptr<Material>;
//using ActorPtr = std::shared_ptr<Actor>;
//
//void framebuffer_size_callback2(GLFWwindow* window, int width, int height);
//void mouse_callback2(GLFWwindow* window, double xpos, double ypos);
//void processInput2(GLFWwindow* window);
//
//// settings
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
//
//// camera
//Camera2 camera(glm::vec3(0.0f, 0.0f, 3.0f));
//float lastX = (float)SCR_WIDTH / 2.0;
//float lastY = (float)SCR_HEIGHT / 2.0;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//int main()
//{
//	// glfw: initialize and configure
//	// ------------------------------
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//	// glfw window creation
//	// --------------------
//	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//	if (window == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback2);
//	glfwSetCursorPosCallback(window, mouse_callback2);
//
//	// tell GLFW to capture our mouse
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	// glad: load all OpenGL function pointers
//	// ---------------------------------------
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		return -1;
//	}
//
//	// configure global opengl state
//	// -----------------------------
//	glEnable(GL_DEPTH_TEST);
//
//	// build and compile shaders
//	// -------------------------
//	std::shared_ptr<ShaderCompiler> shaderRed=std::make_shared<ShaderCompiler>("shaders/UniformBufferTest_05.vs", "shaders/UniformBufferTest_05.fs");
//	shaderRed->Compile();
//
//	// set up vertex data (and buffer(s)) and configure vertex attributes
//	// ------------------------------------------------------------------
//	float cubeVertices[] = {
//		// positions         
//		-0.5f, -0.5f, -0.5f,
//		 0.5f, -0.5f, -0.5f,
//		 0.5f,  0.5f, -0.5f,
//		 0.5f,  0.5f, -0.5f,
//		-0.5f,  0.5f, -0.5f,
//		-0.5f, -0.5f, -0.5f,
//
//		-0.5f, -0.5f,  0.5f,
//		 0.5f, -0.5f,  0.5f,
//		 0.5f,  0.5f,  0.5f,
//		 0.5f,  0.5f,  0.5f,
//		-0.5f,  0.5f,  0.5f,
//		-0.5f, -0.5f,  0.5f,
//
//		-0.5f,  0.5f,  0.5f,
//		-0.5f,  0.5f, -0.5f,
//		-0.5f, -0.5f, -0.5f,
//		-0.5f, -0.5f, -0.5f,
//		-0.5f, -0.5f,  0.5f,
//		-0.5f,  0.5f,  0.5f,
//
//		 0.5f,  0.5f,  0.5f,
//		 0.5f,  0.5f, -0.5f,
//		 0.5f, -0.5f, -0.5f,
//		 0.5f, -0.5f, -0.5f,
//		 0.5f, -0.5f,  0.5f,
//		 0.5f,  0.5f,  0.5f,
//
//		-0.5f, -0.5f, -0.5f,
//		 0.5f, -0.5f, -0.5f,
//		 0.5f, -0.5f,  0.5f,
//		 0.5f, -0.5f,  0.5f,
//		-0.5f, -0.5f,  0.5f,
//		-0.5f, -0.5f, -0.5f,
//
//		-0.5f,  0.5f, -0.5f,
//		 0.5f,  0.5f, -0.5f,
//		 0.5f,  0.5f,  0.5f,
//		 0.5f,  0.5f,  0.5f,
//		-0.5f,  0.5f,  0.5f,
//		-0.5f,  0.5f, -0.5f,
//	};
//	// cube VAO
//	VertexModelPtr cube = std::make_shared<VertexModel>();
//	cube->BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);
//
//	// configure a uniform buffer object
//	// ---------------------------------
//	// first. We get the relevant block indices
//	shaderRed->BlockBindingUniform(0, "Matrices");
//
//	//// Now actually create the buffer
//	//unsigned int uboMatrices;
//	//glGenBuffers(1, &uboMatrices);
//	//glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
//	//glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
//	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	//// define the range of the buffer that links to a uniform binding point
//	//glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
//
//	//// store the projection matrix (we only do this once now) (note: we're not using zoom anymore by changing the FoV)
//	//glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//	//glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
//	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
//	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
//
//	std::shared_ptr<UniformBuffer> matrixBuffer=std::make_shared<UniformBuffer>(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::UNIFORM_BLOCK_NAME::MATRIX_COORD_SYSTEM);
//	UniformData ud1(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX, UniformDataType::Mat4);
//	matrixBuffer->AddData(ud1);
//	UniformData ud2(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX, UniformDataType::Mat4);
//	matrixBuffer->AddData(ud2);
//	matrixBuffer->Create();
//	matrixBuffer->LinkBindingPoint();
//	auto proj = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//	matrixBuffer->StoreData<glm::mat4>(proj, CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX);
//
//	MaterialPtr m = std::make_shared<Material>(shaderRed);
//	ActorPtr a = std::make_shared<Actor>(cube, m);
//
//	// render loop
//	// -----------
//	while (!glfwWindowShouldClose(window))
//	{
//		// per-frame time logic
//		// --------------------
//		float currentFrame = glfwGetTime();
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//		// input
//		// -----
//		processInput2(window);
//
//		// render
//		// ------
//		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// set the view and projection matrix in the uniform block - we only have to do this once per loop iteration.
//		glm::mat4 view = camera.GetViewMatrix();
//		matrixBuffer->StoreData<glm::mat4>(view, CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX);
//
//		// draw 4 cubes 
//		// RED
//		//cube->BindVAO();
//		//shaderRed->UseSelf();
//		//glm::mat4 model = glm::mat4(1.0f);
//		//model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f)); // move top-left
//		//shaderRed->SetMat4("model", model);
//		//cube->Draw();
//		//
//		// 
//		
//		a->Draw();
//
//		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//		// -------------------------------------------------------------------------------
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	// optional: de-allocate all resources once they've outlived their purpose:
//	// ------------------------------------------------------------------------
//	
//
//	glfwTerminate();
//	return 0;
//}
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput2(GLFWwindow* window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		camera.ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		camera.ProcessKeyboard(BACKWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		camera.ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback2(GLFWwindow* window, int width, int height)
//{
//	// make sure the viewport matches the new window dimensions; note that width and 
//	// height will be significantly larger than specified on retina displays.
//	glViewport(0, 0, width, height);
//}
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void mouse_callback2(GLFWwindow* window, double xpos, double ypos)
//{
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//	lastX = xpos;
//	lastY = ypos;
//
//	camera.ProcessMouseMovement(xoffset, yoffset);
//}
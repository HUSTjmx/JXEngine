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
#include "ShaderCompiler.h"
#include "VertexModel.h"

//#define  __MSAA__ 1

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

	std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.8f, -5.0f));
	camera->Front = glm::vec3(0, 0, 1);

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

#ifdef __MSAA__
	glfwWindowHint(GLFW_SAMPLES, 4);
#endif

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

#ifdef MSAA
	glEnable(GL_MULTISAMPLE);
#endif // MSAA

	//glEnable(GL_CULL_FACE);

	// --------------------------------前向渲染 + RayMarching-------------------------------------
	auto p0 = OPENGL_SCENE::TestPass::Intance().GetPass_BackFaceDepth_09_01();

	auto p0_1 = OPENGL_SCENE::TestPass::Intance().GetPass_FrontFaceDepth_09_02();

	auto p1 = OPENGL_SCENE::TestPass::Intance().GetPass_ShadowMap_09();

	auto p2 = OPENGL_SCENE::TestPass::Intance().GetPass_Cloud_11_01(p1, p0);

	auto p3 = OPENGL_SCENE::TestPass::Intance().GetPass_RayMarchingRendering(p2, p0, p0_1);
	// -------------------------------------------------------------------------------------------
	

	// ------------------------------- only RayMarching -------------------------------------------
	auto cloud_shader_01 = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::PURE_CLOUD::Cloud_01_vs.c_str(),
		SHADER_PATH::RAY_MARCHING::PURE_CLOUD::Cloud_01_fs.c_str());
	cloud_shader_01->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	cloud_shader_01->Compile();
	auto cloud_mat_01 = std::make_shared<Material>(cloud_shader_01);
	auto cloud_p_01 = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(cloud_mat_01);
	// --------------------------------------------------------------------------------------------



	//auto p3 = OPENGL_SCENE::TestPass::Intance().GetPass_FoveatedRendering_12(p2);

	//auto p4 = OPENGL_SCENE::TestPass::Intance().GetPass_FoveatedRendering_Pass2_13(p3);

	//auto p_debug = OPENGL_SCENE::TestPass::Intance().GetPass_ShadowDebug_10(p0);

	//时钟重置，开始计时
	Clock.Reset();

	//
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		Clock.Timing();
		Clock.FPS();

		// input
		// -----
		INPUT::processInput(window);

		// render
		// ------
		cloud_p_01->BindOutput();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cloud_p_01->Draw();
		
		//OPENGL_SCENE::TestPass::Intance().DrawFoveated_Comp_09(p0, p0_1, p1, p2, p3);
		//OPENGL_SCENE::TestPass::Intance().DrawFoveated_07(p0, p1, p2);
		//OPENGL_SCENE::TestPass::Intance().DrawBaseShadow_06(p1, p2);
		//OPENGL_SCENE::TestPass::Intance().Draw_FrameTest_04(p1, p2);
		//OPENGL_SCENE::TestPass::Intance().DrawShadowTest_05(p0, p_debug);

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
//#include "learnopengl/model.h"
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
//	Shader asteroidShader("10.3.asteroids.vs", "10.3.asteroids.fs");
//	Shader planetShader("10.3.planet.vs", "10.3.planet.fs");
//
//	// load models
//	// -----------
//	Model rock(FileSystem::getPath("resources/objects/rock/rock.obj"));
//	Model planet(FileSystem::getPath("resources/objects/planet/planet.obj"));
//
//	// generate a large list of semi-random model transformation matrices
//	// ------------------------------------------------------------------
//	unsigned int amount = 100000;
//	glm::mat4* modelMatrices;
//	modelMatrices = new glm::mat4[amount];
//	srand(glfwGetTime()); // initialize random seed	
//	float radius = 150.0;
//	float offset = 25.0f;
//	for (unsigned int i = 0; i < amount; i++)
//	{
//		glm::mat4 model = glm::mat4(1.0f);
//		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
//		float angle = (float)i / (float)amount * 360.0f;
//		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
//		float x = sin(angle) * radius + displacement;
//		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
//		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
//		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
//		float z = cos(angle) * radius + displacement;
//		model = glm::translate(model, glm::vec3(x, y, z));
//
//		// 2. scale: Scale between 0.05 and 0.25f
//		float scale = (rand() % 20) / 100.0f + 0.05;
//		model = glm::scale(model, glm::vec3(scale));
//
//		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
//		float rotAngle = (rand() % 360);
//		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
//
//		// 4. now add to list of matrices
//		modelMatrices[i] = model;
//	}
//
//	// configure instanced array
//	// -------------------------
//	unsigned int buffer;
//	glGenBuffers(1, &buffer);
//	glBindBuffer(GL_ARRAY_BUFFER, buffer);
//	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
//
//	// set transformation matrices as an instance vertex attribute (with divisor 1)
//	// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
//	// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
//	// -----------------------------------------------------------------------------------------------------------------------------------
//	for (unsigned int i = 0; i < rock.meshes.size(); i++)
//	{
//		unsigned int VAO = rock.meshes[i].VAO;
//		glBindVertexArray(VAO);
//		// set attribute pointers for matrix (4 times vec4)
//		glEnableVertexAttribArray(3);
//		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
//		glEnableVertexAttribArray(4);
//		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
//		glEnableVertexAttribArray(5);
//		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
//		glEnableVertexAttribArray(6);
//		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
//
//		glVertexAttribDivisor(3, 1);
//		glVertexAttribDivisor(4, 1);
//		glVertexAttribDivisor(5, 1);
//		glVertexAttribDivisor(6, 1);
//
//		glBindVertexArray(0);
//	}
//
//
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
//		// configure transformation matrices
//		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
//		glm::mat4 view = camera.GetViewMatrix();
//		asteroidShader.use();
//		asteroidShader.setMat4("projection", projection);
//		asteroidShader.setMat4("view", view);
//		planetShader.use();
//		planetShader.setMat4("projection", projection);
//		planetShader.setMat4("view", view);
//
//		// draw planet
//		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
//		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
//		planetShader.setMat4("model", model);
//		planet.Draw(planetShader);
//
//		// draw meteorites
//		asteroidShader.use();
//		asteroidShader.setInt("texture_diffuse1", 0);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
//		for (unsigned int i = 0; i < rock.meshes.size(); i++)
//		{
//			glBindVertexArray(rock.meshes[i].VAO);
//			glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
//			glBindVertexArray(0);
//		}
//	    
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
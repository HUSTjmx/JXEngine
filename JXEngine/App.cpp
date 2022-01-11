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
#include "Light.h"

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

	//std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3(20.0, 18.0, -40.0));
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3(0.0, 0.0, -12.0));
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

void SET_LIGHT(Scene& scene)
{
	auto pointLight0 = std::make_shared<PointLight>(glm::vec3(0.0, 2.0, -1.5));
	pointLight0->SetColor(glm::vec3(1.0, 0.9, 0.5));
	pointLight0->SetRadius(10.0);
	pointLight0->SetValue(200.0);
	scene.AddLight(pointLight0);
}

void Test()
{
	auto project_mat = INPUT::inputCamera->GetProjectionMatrix();
	auto view = INPUT::inputCamera->GetViewMatrix();
	auto ndc_pos = glm::vec3(0.5, 0.5, 1.0);
	ndc_pos = ndc_pos * glm::vec3(2.0) - glm::vec3(1.0);

	auto w_pos = glm::inverse(view) * glm::inverse(project_mat) * (glm::vec4(ndc_pos, 1.0) * glm::vec4(CONFIG::CAMERA_CONFIG::FAR_PLANE));
	JMX_INPUT_GLM(w_pos);
	auto xx = project_mat * view * w_pos;
	ndc_pos = glm::vec3(xx.r,xx.g,xx.b) / xx.w;
	ndc_pos = (ndc_pos + glm::vec3(1.0)) / glm::vec3(2.0);
	JMX_INPUT_GLM(ndc_pos);


	// Test StretchToFarPLane
	auto w_pos2 = glm::vec3(12.0, 0.0, 0.0);
	auto v = glm::normalize(w_pos2 - INPUT::inputCamera->Position);
	float cos_theta = glm::dot(v, INPUT::inputCamera->Front);
	float len = CONFIG::CAMERA_CONFIG::FAR_PLANE / cos_theta;
	glm::vec3 new_pos = INPUT::inputCamera->Position + v * len;
	JMX_INPUT_GLM(new_pos);
}

void Loop(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);

#ifdef MSAA
	glEnable(GL_MULTISAMPLE);
#endif // MSAA

	//glEnable(GL_CULL_FACE);

	// --------------------------------前向渲染 + RayMarching-------------------------------------
	/*auto p0 = OPENGL_SCENE::TestPass::Intance().GetPass_BackFaceDepth_09_01();

	auto p0_1 = OPENGL_SCENE::TestPass::Intance().GetPass_FrontFaceDepth_09_02();

	auto p1 = OPENGL_SCENE::TestPass::Intance().GetPass_ShadowMap_09();

	auto p2 = OPENGL_SCENE::TestPass::Intance().GetPass_Cloud_11_01(p1, p0);

	auto p3 = OPENGL_SCENE::TestPass::Intance().GetPass_RayMarchingRendering(p2, p0, p0_1);*/
	// -------------------------------------------------------------------------------------------
	

	// ------------------------------- only RayMarching -------------------------------------------
	auto noisyTex = std::make_shared<Texture>(0, ASSETS::TEXTURE::NOISY::Gray_Noise_Medium.c_str(), "noisyTexture", GL_REPEAT);
	auto preTex = std::make_shared<Texture>("preTexture");

	auto frame = std::make_shared<FrameBuffer>(CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	frame->AddTexture(GL_RGB, "screenTexture", false);
	frame->AddTexture(GL_RGB, "preTexture", false);
	frame->AddTexture(GL_RGBA32F, "scatterTex", false);
	frame->AddTexture(GL_RGBA32F, "posTex", false);
	frame->NotifyGL();
	frame->AddRenderObject(true);

#pragma region CloudScene_MAT_0
	auto cloud_shader_01 = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::PURE_CLOUD::Cloud_02_vs.c_str(),
		SHADER_PATH::RAY_MARCHING::PURE_CLOUD::Cloud_02_fs.c_str());
	cloud_shader_01->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	cloud_shader_01->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	cloud_shader_01->Compile();
	auto cloud_mat_01 = std::make_shared<Material>(cloud_shader_01);
	cloud_mat_01->AddTexture(noisyTex);
	cloud_mat_01->AddTexture(frame->textureBuffers[1]);
	cloud_mat_01->AddTexture(frame->textureBuffers[2]);
	cloud_mat_01->AddTexture(frame->textureBuffers[3]);
	cloud_mat_01->LinkTextureForShader();
	cloud_mat_01->SetJitter(true);
#pragma endregion

#pragma region MysteryMountains_MAT
	auto MysteryMoutains_shader = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::FBM_GREAT_EFFECT::MysteryMoutains_vs.c_str(),
		SHADER_PATH::RAY_MARCHING::FBM_GREAT_EFFECT::MysteryMoutains_fs.c_str());
	MysteryMoutains_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	MysteryMoutains_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	MysteryMoutains_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	MysteryMoutains_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	MysteryMoutains_shader->Compile();
	auto MysteryMoutains_mat = std::make_shared<Material>(MysteryMoutains_shader);
	MysteryMoutains_mat->AddTexture(noisyTex);
	MysteryMoutains_mat->AddTexture(preTex);
	MysteryMoutains_mat->LinkTextureForShader();
#pragma endregion

#pragma region PlayingMarble
	auto PlayingMarble_Shader = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::MEDIA::PlayingMarble_vs.c_str(),
		SHADER_PATH::RAY_MARCHING::MEDIA::PlayingMarble_fs.c_str());
	PlayingMarble_Shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	PlayingMarble_Shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	PlayingMarble_Shader->Compile();
	auto PlayingMarble_mat = std::make_shared<Material>(PlayingMarble_Shader);
	// Todo. Add history texture.
	cloud_mat_01->AddTexture(frame->textureBuffers[1]);
	cloud_mat_01->AddTexture(frame->textureBuffers[2]);
	cloud_mat_01->AddTexture(frame->textureBuffers[3]);
	PlayingMarble_mat->LinkTextureForShader();
	PlayingMarble_mat->SetJitter(false);
#pragma endregion

#pragma region StaicScene_01
	auto StaticScene_01_shader = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::PAPER::StaticScene_01_vs.c_str(),
		SHADER_PATH::RAY_MARCHING::PAPER::StaticScene_01_fs.c_str());
	StaticScene_01_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	StaticScene_01_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	StaticScene_01_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	StaticScene_01_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	StaticScene_01_shader->Compile();
	auto StaticScene_01_Mat = std::make_shared<Material>(StaticScene_01_shader);
	StaticScene_01_Mat->AddTexture(frame->textureBuffers[1]);
	StaticScene_01_Mat->AddTexture(frame->textureBuffers[2]);
	StaticScene_01_Mat->AddTexture(frame->textureBuffers[3]);
	StaticScene_01_Mat->LinkTextureForShader();
	StaticScene_01_Mat->SetJitter(false);
#pragma endregion

	auto cloud_p_01 = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(StaticScene_01_Mat);
	cloud_p_01->UpdateOutput(frame);
	SET_LIGHT(*cloud_p_01->scene);


#pragma region Foveated

	auto Foveated_P1_Shader = std::make_shared<ShaderCompiler>(SHADER_PATH::POST_RENDER::FOVEA::KernelFovea_vs.c_str(), SHADER_PATH::POST_RENDER::FOVEA::KernelFovea_fs.c_str());
	Foveated_P1_Shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::POST_PROCESSING);
	Foveated_P1_Shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	Foveated_P1_Shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	Foveated_P1_Shader->Compile();
	Foveated_P1_Shader->UseSelf();
	glm::vec2 re = glm::vec2((float)CONFIG::SCREEN_CONFIG::FOVEA::FOVEA_WIDTH, (float)CONFIG::SCREEN_CONFIG::FOVEA::FOVEA_HEIGHT);
	Foveated_P1_Shader->SetVec2(CONFIG::MATERIAL_SETTINGS::FOVEA_RENDER::RESOLUTION_NAME, re);
	auto Foveated_P1_mat = std::make_shared<Material>(Foveated_P1_Shader);
	Foveated_P1_mat->LinkTextureForShader();
	auto Foveated_pass_1 = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(cloud_p_01, Foveated_P1_mat);

	auto Foveated_P2_Shader = std::make_shared<ShaderCompiler>(SHADER_PATH::POST_RENDER::FOVEA::KernelFovea_vs.c_str(), SHADER_PATH::POST_RENDER::FOVEA::KernelFovea_Pass2_fs.c_str());
	Foveated_P2_Shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::POST_PROCESSING);
	Foveated_P2_Shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	Foveated_P2_Shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	Foveated_P2_Shader->Compile();
	Foveated_P2_Shader->UseSelf();
	Foveated_P2_Shader->SetVec2(CONFIG::MATERIAL_SETTINGS::FOVEA_RENDER::RESOLUTION_NAME, re);
	auto Foveated_P2_mat = std::make_shared<Material>(Foveated_P2_Shader);
	Foveated_P2_mat->LinkTextureForShader();
	auto Foveated_pass_2 = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(Foveated_pass_1, Foveated_P2_mat);
#pragma endregion


#pragma region Blur
	// ------------------------------- Foveated Blur ----------------------------------------------
	auto foveated_blur_shader = std::make_shared<ShaderCompiler>(SHADER_PATH::POST_RENDER::BLUR::FoveatedBlur_vs.c_str(), SHADER_PATH::POST_RENDER::BLUR::FoveatedBlur_fs.c_str());
	foveated_blur_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::POST_PROCESSING);
	foveated_blur_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	foveated_blur_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	foveated_blur_shader->Compile();
	foveated_blur_shader->SetInt("blurDir", 1);
	auto foveated_blur_mat = std::make_shared<Material>(foveated_blur_shader);
	foveated_blur_mat->LinkTextureForShader();
	auto foveated_blur_pass_X = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(Foveated_pass_2, foveated_blur_mat);
	auto foveated_blur_mat_2 = foveated_blur_mat->Copy();
	auto foveated_blur_pass_Y = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(foveated_blur_pass_X, foveated_blur_mat_2);

	auto frame2 = std::make_shared<FrameBuffer>(CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	frame2->AddTexture(GL_RGB, "screenTexture", false);
	frame2->NotifyGL();
	frame2->AddRenderObject(true);
	foveated_blur_pass_Y->UpdateOutput(frame2);
	// --------------------------------------------------------------------------------------------
#pragma endregion

	//时钟重置，开始计时
	Clock.Reset();

	//
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	const int blurTimes = 0;

	//Test();

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		Clock.Timing();
		COUNTER(0);
		//Clock.FPS();

		// input
		// -----
		INPUT::processInput(window);

		// render
		// ------
		//std::cout << Counter<0>::times<< std::endl;
		cloud_p_01->BindOutput();
		cloud_p_01->GetMat()->GetShader()->SetInt("IsFirstFrame", Counter<0>::times);
		glClear(GL_DEPTH_BUFFER_BIT);
		cloud_p_01->Draw();

		//cloud_p_01->GetOutput()->CopyColorAttachmentToTex(preTex->Self(), 1);

		Foveated_pass_1->BindOutput();
		glClear(GL_DEPTH_BUFFER_BIT);
		Foveated_pass_1->Draw();
		
		Foveated_pass_2->BindOutput();
		glClear(GL_DEPTH_BUFFER_BIT);
		Foveated_pass_2->Draw();

		foveated_blur_pass_X->BindOutput();
		foveated_blur_pass_X->GetMat()->GetShader()->SetInt("blurDir", 1);
		glClear(GL_DEPTH_BUFFER_BIT);
		foveated_blur_pass_X->Draw();
		
		for (int i = 0;i < blurTimes;++i)
		{
			if (i % 2 == 0)
			{
				foveated_blur_pass_Y->BindOutput(foveated_blur_pass_X->GetOutput());
				foveated_blur_pass_Y->GetMat()->GetShader()->SetInt("blurDir", 0);
				glClear(GL_DEPTH_BUFFER_BIT);
				foveated_blur_pass_Y->Draw();
			}
			else {
				foveated_blur_pass_X->BindOutput(foveated_blur_pass_Y->GetOutput());
				foveated_blur_pass_X->GetMat()->GetShader()->SetInt("blurDir", 1);
				glClear(GL_DEPTH_BUFFER_BIT);
				foveated_blur_pass_X->Draw();
			}
		}

		foveated_blur_pass_Y->BindOutput(0);
		foveated_blur_pass_Y->GetMat()->GetShader()->SetInt("blurDir", 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		foveated_blur_pass_Y->Draw();

		//OPENGL_SCENE::TestPass::Intance().DrawFoveated_Comp_09(p0, p0_1, p1, p2, p3);
		//OPENGL_SCENE::TestPass::Intance().DrawFoveated_07(p0, p1, p2);
		//OPENGL_SCENE::TestPass::Intance().DrawBaseShadow_06(p1, p2);
		//OPENGL_SCENE::TestPass::Intance().Draw_FrameTest_04(p1, p2);
		//OPENGL_SCENE::TestPass::Intance().DrawShadowTest_05(p0, p_debug);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		INPUT::inputCamera->UpdatePreMat();
		INPUT::inputCamera->UpdatePreAttr();

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
}


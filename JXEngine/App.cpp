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
#include "TFRM_Algorithm.h"

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
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3(0.0, 0.0, -14.0));
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

	GLint n = 0, i;
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
	//auto pointLight0 = std::make_shared<PointLight>(glm::vec3(1.0, 1.0, 1.0));
	pointLight0->SetColor(glm::vec3(1.0, 0.9, 0.5));
	pointLight0->SetRadius(10.0);
	pointLight0->SetValue(60.0);
	scene.AddLight(pointLight0);


	glm::vec3 lightPos(0.2f, 0.01f, 1.0f);
	auto dir1 = std::make_shared<DirectionalLight>(-lightPos);
	dir1->color = glm::vec3(1.0, 1.0, 0.8);
	dir1->value = 1.6;
	scene.AddLight(dir1);
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

// 0 : Sky
// 1 : Fog Ball
// 2 : Standford Rabbits
// 3 : Cloud Sea
#define SCENE_ID 1
#define OBJ_ID 2

// 0 : Origin Method
// 1 : Temporal Method
// 2 : Foveated Method
// 3 : Full Method
#define METHOD_ID 3

// false : No Shadow
// true : Shadow
#define SHADOW_ID true

// 0 : Constant Phase Function
// 1 : RAYLEIGH Phase Function
// 2 : MIE Phase Function
// 3 : Blend Function
#define PHASE_ID 2
#define PHASE_OP 0.5

// FOVEATED_REGION_BOUND 
// Default : 5.79
#define FOVEATED_BOUND 5.79

// 用于测试输出
#define TEST_NO_BLUR_AND_FOVEAL 0
#define ONLY_RENDER_ONE_TURN 0

// 是否计算质量结果
#define COMPUTER_QUALITY_ERROR 0

// 是否计算帧率
// 不要使用此宏定义，因为用这种方法计算Pass的消耗时间是错的，我们应该去使用RenderDoc抓帧来获取数据
#define COMPUTER_TIME 0
#define SHOW_FPS 0

// 是否开启Gauss Blur
#define USE_GAUSS_BLUR 1

void Loop(GLFWwindow* window)
{
	glEnable(GL_DEPTH_TEST);

#ifdef MSAA
	glEnable(GL_MULTISAMPLE);
#endif // MSAA

	//glEnable(GL_CULL_FACE);

	auto frame = std::make_shared<FrameBuffer>(CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	frame->AddTexture(GL_RGB, "screenTex", false);
#if METHOD_ID == 1 || METHOD_ID == 3
	frame->AddTexture(GL_RGBA32F, "ScatterLight_In", false);
	frame->AddTexture(GL_RGBA32F, "Transmittance_In", false);
	frame->AddTexture(GL_RGBA32F, "FinalPos_In", false);
#endif
	frame->NotifyGL();
	frame->AddRenderObject(true);

#pragma region CloudScene_MAT_0
	/*auto cloud_shader_01 = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::PURE_CLOUD::Cloud_02_vs.c_str(),
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
	cloud_mat_01->SetJitter(true);*/
#pragma endregion

#pragma region MysteryMountains_MAT
	/*auto MysteryMoutains_shader = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::FBM_GREAT_EFFECT::MysteryMoutains_vs.c_str(),
		SHADER_PATH::RAY_MARCHING::FBM_GREAT_EFFECT::MysteryMoutains_fs.c_str());
	MysteryMoutains_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	MysteryMoutains_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	MysteryMoutains_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	MysteryMoutains_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	MysteryMoutains_shader->Compile();
	auto MysteryMoutains_mat = std::make_shared<Material>(MysteryMoutains_shader);
	MysteryMoutains_mat->AddTexture(noisyTex);
	MysteryMoutains_mat->AddTexture(preTex);
	MysteryMoutains_mat->LinkTextureForShader();*/
#pragma endregion

#pragma region PlayingMarble
	//auto PlayingMarble_Shader = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::MEDIA::PlayingMarble_vs.c_str(),
	//	SHADER_PATH::RAY_MARCHING::MEDIA::PlayingMarble_fs.c_str());
	//PlayingMarble_Shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	//PlayingMarble_Shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	//PlayingMarble_Shader->Compile();
	//auto PlayingMarble_mat = std::make_shared<Material>(PlayingMarble_Shader);
	//// Todo. Add history texture.
	//cloud_mat_01->AddTexture(frame->textureBuffers[1]);
	//cloud_mat_01->AddTexture(frame->textureBuffers[2]);
	//cloud_mat_01->AddTexture(frame->textureBuffers[3]);
	//PlayingMarble_mat->LinkTextureForShader();
	//PlayingMarble_mat->SetJitter(false);
#pragma endregion

#pragma region StaicScene_01
//	auto StaticScene_01_shader = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::PAPER::StaticScene_01_vs.c_str(),
//		SHADER_PATH::RAY_MARCHING::PAPER::StaticScene_01_fs.c_str());
//	StaticScene_01_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
//	StaticScene_01_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
//	StaticScene_01_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
//	StaticScene_01_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
//	StaticScene_01_shader->Compile();
//	auto StaticScene_01_Mat = std::make_shared<Material>(StaticScene_01_shader);
//	StaticScene_01_Mat->AddTexture(frame->textureBuffers[1]);
//	StaticScene_01_Mat->AddTexture(frame->textureBuffers[2]);
//	StaticScene_01_Mat->AddTexture(frame->textureBuffers[3]);
//	StaticScene_01_Mat->LinkTextureForShader();
//	StaticScene_01_Mat->SetJitter(false);
//#pragma endregion

#pragma region StaticScene_02
	/*auto StaticScene_02_shader = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::PAPER::StaticScene_02_vs.c_str(),
		SHADER_PATH::RAY_MARCHING::PAPER::StaticScene_02_fs.c_str());
	StaticScene_02_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	StaticScene_02_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	StaticScene_02_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	StaticScene_02_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	StaticScene_02_shader->Compile();
	auto StaticScene_02_Mat = std::make_shared<Material>(StaticScene_02_shader);
	StaticScene_02_Mat->AddTexture(frame->textureBuffers[1]);
	StaticScene_02_Mat->AddTexture(frame->textureBuffers[2]);
	StaticScene_02_Mat->AddTexture(frame->textureBuffers[3]);
	StaticScene_02_Mat->LinkTextureForShader();
	StaticScene_02_Mat->SetJitter(false);*/
#pragma endregion

//#pragma region StaticScene_03
//	auto StaticScene_03_shader = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::PURE_CLOUD::Cloud_01_vs.c_str(),
//		SHADER_PATH::RAY_MARCHING::PURE_CLOUD::Cloud_01_fs.c_str());
//	StaticScene_03_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
//	StaticScene_03_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
//	StaticScene_03_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
//	StaticScene_03_shader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
//	StaticScene_03_shader->Compile();
//	auto StaticScene_03_Mat = std::make_shared<Material>(StaticScene_03_shader);
//	StaticScene_03_Mat->AddTexture(frame->textureBuffers[1]);
//	StaticScene_03_Mat->AddTexture(frame->textureBuffers[2]);
//	StaticScene_03_Mat->AddTexture(frame->textureBuffers[3]);
//	StaticScene_03_Mat->LinkTextureForShader();
//	StaticScene_03_Mat->SetJitter(false);
//#pragma endregion

    // Main Shader
	auto PaperScene_Show = std::make_shared<Material>();

	//Paper Scene 01 : Sky atmosphere
#pragma region PaperScene_01_Sky
#if SCENE_ID == 0
	auto sky_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_01_Sky_VS.c_str(), SHADER_PATH::PAPER::PaperScene_01_Sky_FS.c_str());
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	sky_sh->Compile();
	auto PaperScene_01_Sky = std::make_shared<Material>(sky_sh);
	PaperScene_01_Sky->SetJitter(false);
	PaperScene_01_Sky->Active();
	//std::cout << earthRadius << std::endl;
	/*PaperScene_01_Sky->GetShader()->SetFloat(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::EARTH_RADIUS, 6360e3);
	PaperScene_01_Sky->GetShader()->SetFloat(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::ATMOSHERE_RADIUS, 6420e3);
	PaperScene_01_Sky->GetShader()->SetFloat(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::HR, 7994);
	PaperScene_01_Sky->GetShader()->SetFloat(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::HM, 1200);
	PaperScene_01_Sky->GetShader()->SetVec3(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::BETA_R, glm::vec3(3.8e-6f, 13.5e-6f, 33.1e-6f));
	PaperScene_01_Sky->GetShader()->SetVec3(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::BETA_M, glm::vec3(21e-6f));
	PaperScene_01_Sky->Active();*/
	PaperScene_Show = PaperScene_01_Sky->CopyWithTex();
#endif 
#pragma endregion

	//Paper Scene 02: Fog Ball, case : N 
#pragma region PaperScene_02_FogBall_N
#if SCENE_ID == 1 && METHOD_ID == 0
	// 01
	auto FB_01_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_02_FogBall_VS.c_str(), SHADER_PATH::PAPER::PaperScene_02_FogBall_01_N_FS.c_str());
	// 02
	//auto FB_01_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_02_FogBall_VS.c_str(), SHADER_PATH::PAPER::PaperScene_02_FogBall_02_N_FS.c_str());
	FB_01_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	FB_01_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	FB_01_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	FB_01_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	FB_01_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	FB_01_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	FB_01_sh->AddMacroDefine("IS_SHOW_SHADOW", SHADOW_ID);
	FB_01_sh->AddMacroDefine("OBJ_TYPE", OBJ_ID);
	FB_01_sh->Compile();
	auto PaperScene_02_FogBall_01 = std::make_shared<Material>(FB_01_sh);
	PaperScene_02_FogBall_01->SetJitter(false);
	PaperScene_02_FogBall_01->Active();

	PaperScene_Show = PaperScene_02_FogBall_01->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 02: Fog Ball, case : T
#pragma region PaperScene_02_FogBall_T
#if SCENE_ID == 1 && METHOD_ID == 1
	// 01
	//auto FB_02_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_02_FogBall_VS.c_str(), SHADER_PATH::PAPER::PaperScene_02_FogBall_01_N_FS.c_str());
	// 02
	auto FB_02_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_02_FogBall_VS.c_str(), SHADER_PATH::PAPER::PaperScene_02_FogBall_01_T_FS.c_str());
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	FB_02_sh->AddMacroDefine("IS_SHOW_SHADOW", SHADOW_ID);
	FB_02_sh->AddMacroDefine("OBJ_TYPE", OBJ_ID);
	FB_02_sh->Compile();
	auto PaperScene_02_FogBall_T = std::make_shared<Material>(FB_02_sh);
	PaperScene_02_FogBall_T->AddTexture(frame->textureBuffers[1]);
	PaperScene_02_FogBall_T->AddTexture(frame->textureBuffers[2]);
	PaperScene_02_FogBall_T->AddTexture(frame->textureBuffers[3]);
	PaperScene_02_FogBall_T->LinkTextureForShader();
	PaperScene_02_FogBall_T->SetJitter(false);
	PaperScene_02_FogBall_T->Active();

	PaperScene_Show = PaperScene_02_FogBall_T->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 02: Fog Ball, case : S
#pragma region PaperScene_02_FogBall_S
#if SCENE_ID == 1 && METHOD_ID == 2
	// 01
	//auto FB_02_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_02_FogBall_VS.c_str(), SHADER_PATH::PAPER::PaperScene_02_FogBall_01_N_FS.c_str());
	// 02
	auto FB_02_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_02_FogBall_VS.c_str(), SHADER_PATH::PAPER::PaperScene_02_FogBall_01_S_FS.c_str());
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	FB_02_sh->AddMacroDefine("IS_SHOW_SHADOW", SHADOW_ID);
	FB_02_sh->AddMacroDefine("PHASE_FUNCTION_MEDIA", PHASE_ID);
	FB_02_sh->AddMacroDefine("PHASE_FUNCTION_BLEND_OP", std::to_string(PHASE_OP));
	FB_02_sh->AddMacroDefine("OBJ_TYPE", OBJ_ID);
	FB_02_sh->Compile();
	auto PaperScene_02_FogBall_S = std::make_shared<Material>(FB_02_sh);
	PaperScene_02_FogBall_S->SetJitter(false);
	PaperScene_02_FogBall_S->Active();

	PaperScene_Show = PaperScene_02_FogBall_S->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 02: Fog Ball, case : ST
#pragma region PaperScene_02_FogBall_ST
#if SCENE_ID == 1 && METHOD_ID == 3
	// 01
	//auto FB_02_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_02_FogBall_VS.c_str(), SHADER_PATH::PAPER::PaperScene_02_FogBall_01_N_FS.c_str());
	// 02
	auto FB_02_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_02_FogBall_VS.c_str(), SHADER_PATH::PAPER::PaperScene_02_FogBall_01_ST_FS.c_str());
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	FB_02_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	FB_02_sh->AddMacroDefine("IS_SHOW_SHADOW", SHADOW_ID);
	FB_02_sh->AddMacroDefine("PHASE_FUNCTION_MEDIA", PHASE_ID);
	FB_02_sh->AddMacroDefine("PHASE_FUNCTION_BLEND_OP", std::to_string(PHASE_OP));
	FB_02_sh->AddMacroDefine("OBJ_TYPE", OBJ_ID);
	FB_02_sh->Compile();
	auto PaperScene_02_FogBall_T = std::make_shared<Material>(FB_02_sh);
	PaperScene_02_FogBall_T->AddTexture(frame->textureBuffers[1]);
	PaperScene_02_FogBall_T->AddTexture(frame->textureBuffers[2]);
	PaperScene_02_FogBall_T->AddTexture(frame->textureBuffers[3]);
	PaperScene_02_FogBall_T->LinkTextureForShader();
	PaperScene_02_FogBall_T->SetJitter(false);
	PaperScene_02_FogBall_T->Active();

	PaperScene_Show = PaperScene_02_FogBall_T->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 03: Stanford Rabbits, case : N
#pragma region PaperScene_03_StanfordRabbits_N
#if SCENE_ID == 2 && METHOD_ID == 0
	auto SR_01_sh_N = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_03_StanfordRabbits_VS.c_str(), SHADER_PATH::PAPER::PaperScene_03_StanfordRabbits_01_N_FS.c_str());
	SR_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	SR_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	SR_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	SR_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	SR_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	SR_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	SR_01_sh_N->AddMacroDefine("IS_SHOW_SHADOW", SHADOW_ID);
	SR_01_sh_N->Compile();
	auto PaperScene_03_StanfordRabbits_01_N = std::make_shared<Material>(SR_01_sh_N);
	PaperScene_03_StanfordRabbits_01_N->SetJitter(false);
	PaperScene_03_StanfordRabbits_01_N->Active();

	PaperScene_Show = PaperScene_03_StanfordRabbits_01_N->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 03: Stanford Rabbits, case : T
#pragma region PaperScene_03_StanfordRabbits_T
#if SCENE_ID == 2 && METHOD_ID == 1
	auto SR_01_sh_T = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_03_StanfordRabbits_VS.c_str(), SHADER_PATH::PAPER::PaperScene_03_StanfordRabbits_01_T_FS.c_str());
	SR_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	SR_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	SR_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	SR_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	SR_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	SR_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	SR_01_sh_T->AddMacroDefine("IS_SHOW_SHADOW", SHADOW_ID);
	SR_01_sh_T->Compile();
	auto PaperScene_03_StanfordRabbits_01_T = std::make_shared<Material>(SR_01_sh_T);
	PaperScene_03_StanfordRabbits_01_T->AddTexture(frame->textureBuffers[1]);
	PaperScene_03_StanfordRabbits_01_T->AddTexture(frame->textureBuffers[2]);
	PaperScene_03_StanfordRabbits_01_T->AddTexture(frame->textureBuffers[3]);
	PaperScene_03_StanfordRabbits_01_T->LinkTextureForShader();
	PaperScene_03_StanfordRabbits_01_T->SetJitter(false);
	PaperScene_03_StanfordRabbits_01_T->Active();

	PaperScene_Show = PaperScene_03_StanfordRabbits_01_T->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 03: Stanford Rabbits, case : S
#pragma region PaperScene_03_StanfordRabbits_S
#if SCENE_ID == 2 && METHOD_ID == 2
	auto SR_01_sh_S = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_03_StanfordRabbits_VS.c_str(), SHADER_PATH::PAPER::PaperScene_03_StanfordRabbits_01_S_FS.c_str());
	SR_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	SR_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	SR_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	SR_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	SR_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	SR_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	SR_01_sh_S->AddMacroDefine("IS_SHOW_SHADOW", SHADOW_ID);
	SR_01_sh_S->AddMacroDefine("PHASE_FUNCTION_MEDIA", PHASE_ID);
	SR_01_sh_S->AddMacroDefine("PHASE_FUNCTION_BLEND_OP", std::to_string(PHASE_OP));
	SR_01_sh_S->Compile();
	auto PaperScene_03_StanfordRabbits_01_S = std::make_shared<Material>(SR_01_sh_S);
	PaperScene_03_StanfordRabbits_01_S->SetJitter(false);
	PaperScene_03_StanfordRabbits_01_S->Active();
	PaperScene_Show = PaperScene_03_StanfordRabbits_01_S->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 03: Stanford Rabbits, case : ST
#pragma region PaperScene_03_StanfordRabbits_ST
#if SCENE_ID == 2 && METHOD_ID == 3
	auto SR_01_sh_ST = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_03_StanfordRabbits_VS.c_str(), SHADER_PATH::PAPER::PaperScene_03_StanfordRabbits_01_T_FS.c_str());
	SR_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	SR_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	SR_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	SR_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	SR_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	SR_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	SR_01_sh_ST->AddMacroDefine("IS_SHOW_SHADOW", SHADOW_ID);
	SR_01_sh_ST->AddMacroDefine("PHASE_FUNCTION_MEDIA", PHASE_ID);
	SR_01_sh_ST->AddMacroDefine("PHASE_FUNCTION_BLEND_OP", std::to_string(PHASE_OP));
	SR_01_sh_ST->Compile();
	auto PaperScene_03_StanfordRabbits_01_ST = std::make_shared<Material>(SR_01_sh_ST);
	PaperScene_03_StanfordRabbits_01_ST->AddTexture(frame->textureBuffers[1]);
	PaperScene_03_StanfordRabbits_01_ST->AddTexture(frame->textureBuffers[2]);
	PaperScene_03_StanfordRabbits_01_ST->AddTexture(frame->textureBuffers[3]);
	PaperScene_03_StanfordRabbits_01_ST->LinkTextureForShader();
	PaperScene_03_StanfordRabbits_01_ST->SetJitter(false);
	PaperScene_03_StanfordRabbits_01_ST->Active();

	PaperScene_Show = PaperScene_03_StanfordRabbits_01_ST->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 04: Cloud of Sea, case : N
#pragma region PaperScene_04_CloudOfSea_N
#if SCENE_ID == 3 && METHOD_ID == 0
	auto SoC_01_sh_N = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_04_CloudOfSea_VS.c_str(), SHADER_PATH::PAPER::PaperScene_04_CloudOfSea_01_N_FS.c_str());
	SoC_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	SoC_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	SoC_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	SoC_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	SoC_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	SoC_01_sh_N->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	SoC_01_sh_N->AddMacroDefine("IS_SHOW_SHADOW", SHADOW_ID);
	SoC_01_sh_N->Compile();
	auto PaperScene_04_CloudOfSea_01_N = std::make_shared<Material>(SoC_01_sh_N);
	PaperScene_04_CloudOfSea_01_N->SetJitter(false);
	PaperScene_04_CloudOfSea_01_N->Active();

	PaperScene_Show = PaperScene_04_CloudOfSea_01_N->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 04: Cloud of Sea, case : T
#pragma region PaperScene_04_CloudOfSea_T
#if SCENE_ID == 3 && METHOD_ID == 1
	auto SoC_01_sh_T = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_04_CloudOfSea_VS.c_str(), SHADER_PATH::PAPER::PaperScene_04_CloudOfSea_01_T_FS.c_str());
	SoC_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	SoC_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	SoC_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	SoC_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	SoC_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	SoC_01_sh_T->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	SoC_01_sh_T->Compile();
	auto PaperScene_04_CloudOfSea_01_T = std::make_shared<Material>(SoC_01_sh_T);
	PaperScene_04_CloudOfSea_01_T->AddTexture(frame->textureBuffers[1]);
	PaperScene_04_CloudOfSea_01_T->AddTexture(frame->textureBuffers[2]);
	PaperScene_04_CloudOfSea_01_T->AddTexture(frame->textureBuffers[3]);
	PaperScene_04_CloudOfSea_01_T->LinkTextureForShader();
	PaperScene_04_CloudOfSea_01_T->SetJitter(false);
	PaperScene_04_CloudOfSea_01_T->Active();

	PaperScene_Show = PaperScene_04_CloudOfSea_01_T->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 04: Cloud of Sea, case : S
#pragma region PaperScene_04_CloudOfSea_S
#if SCENE_ID == 3 && METHOD_ID == 2
	auto SoC_01_sh_S = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_04_CloudOfSea_VS.c_str(), SHADER_PATH::PAPER::PaperScene_04_CloudOfSea_01_S_FS.c_str());
	SoC_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	SoC_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	SoC_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	SoC_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	SoC_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	SoC_01_sh_S->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	SoC_01_sh_S->Compile();
	auto PaperScene_04_CloudOfSea_01_S = std::make_shared<Material>(SoC_01_sh_S);
	PaperScene_04_CloudOfSea_01_S->SetJitter(false);
	PaperScene_04_CloudOfSea_01_S->Active();

	PaperScene_Show = PaperScene_04_CloudOfSea_01_S->CopyWithTex();
#endif
#pragma endregion

	//Paper Scene 04: Cloud of Sea, case : ST
#pragma region PaperScene_04_CloudOfSea_ST
#if SCENE_ID == 3 && METHOD_ID == 3
	auto SoC_01_sh_ST = std::make_shared<ShaderCompiler>(SHADER_PATH::PAPER::PaperScene_04_CloudOfSea_VS.c_str(), SHADER_PATH::PAPER::PaperScene_04_CloudOfSea_01_ST_FS.c_str());
	SoC_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::PAPER);
	SoC_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	SoC_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	SoC_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	SoC_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	SoC_01_sh_ST->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	SoC_01_sh_ST->Compile();
	auto PaperScene_04_CloudOfSea_01_ST = std::make_shared<Material>(SoC_01_sh_ST);
	PaperScene_04_CloudOfSea_01_ST->AddTexture(frame->textureBuffers[1]);
	PaperScene_04_CloudOfSea_01_ST->AddTexture(frame->textureBuffers[2]);
	PaperScene_04_CloudOfSea_01_ST->AddTexture(frame->textureBuffers[3]);
	PaperScene_04_CloudOfSea_01_ST->LinkTextureForShader();
	PaperScene_04_CloudOfSea_01_ST->SetJitter(false);
	PaperScene_04_CloudOfSea_01_ST->Active();

	PaperScene_Show = PaperScene_04_CloudOfSea_01_ST->CopyWithTex();
#endif
#pragma endregion


	//bool useSky = true;

	// Paper Scene 01 : Sky atmosphere
	// auto cloud_p_01 = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(PaperScene_01_Sky);
	// Paper Scene 02 - 01: Fog Ball
	auto mainScenePass = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(PaperScene_Show);

	//if(useSky) cloud_p_01 = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(StaticScene_04_Mat);
	mainScenePass->UpdateOutput(frame);
	//if(useSky) cloud_p_01->scene->MainCamera().Position = glm::vec3(0.0, 6360e3 + 1.0, 0.0);
	SET_LIGHT(*mainScenePass->scene);


#pragma region Foveated
#if (METHOD_ID == 2 || METHOD_ID == 3) && TEST_NO_BLUR_AND_FOVEAL == 0
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
	auto Foveated_pass_1 = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(mainScenePass, Foveated_P1_mat, (float)CONFIG::SCREEN_CONFIG::FOVEA::FOVEA_WIDTH, (float)CONFIG::SCREEN_CONFIG::FOVEA::FOVEA_HEIGHT);

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
#endif
#pragma endregion


#pragma region Blur
#if (METHOD_ID == 2 || METHOD_ID == 3) && TEST_NO_BLUR_AND_FOVEAL == 0 && USE_GAUSS_BLUR == 1
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
#endif
#pragma endregion

#pragma region ColorGrading
#if METHOD_ID == 0 || METHOD_ID == 1 || TEST_NO_BLUR_AND_FOVEAL == 1
	auto origin_show_shader = std::make_shared<ShaderCompiler>(SHADER_PATH::POST_RENDER::ColorGrading::OriginShow_vs.c_str(), SHADER_PATH::POST_RENDER::ColorGrading::OriginShow_fs.c_str());
	origin_show_shader->Compile();
	auto origin_show_mat = std::make_shared<Material>(origin_show_shader);
	origin_show_mat->LinkTextureForShader();
	auto origin_show_pass = OPENGL_SCENE::PostPassFactory::Instance().CreateOne(mainScenePass, origin_show_mat);
#endif
#pragma endregion

	//时钟重置，开始计时
	Clock.Reset();

	//
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	const int blurTimes = 0;
	

#if METHOD_ID == 1 || METHOD_ID == 3
	float F_dis, F_ang = 0.0;
	mainScenePass->GetMat()->GetShader()->SetFloat(Algorithm::TFRM::UNIFORM_NAME::TEMPORAL_ACCELERATION_MUL_NAME, 1.0 / Algorithm::TFRM::Setting::TEMPORAL_ACCELERATION_MUL);
#endif

#if METHOD_ID == 2 || METHOD_ID == 3 || METHOD_ID == 1
	mainScenePass->GetMat()->GetShader()->SetFloat(CONFIG::PAPER_DEFAULT_SETTINGS::FOVEATED_REGION_BOUND, FOVEATED_BOUND);
#endif


	mainScenePass->GetMat()->GetShader()->SetFloat(Algorithm::TFRM::UNIFORM_NAME::MAX_STEP_NAME, Algorithm::TFRM::Setting::MAX_STEP_NUM);
	mainScenePass->GetMat()->GetShader()->SetFloat(Algorithm::TFRM::UNIFORM_NAME::MIN_STEP_NAME, Algorithm::TFRM::Setting::MIN_STEP_NUM);
	mainScenePass->GetMat()->GetShader()->SetFloat(Algorithm::TFRM::UNIFORM_NAME::MAX_SHADOW_STEP_NAME, Algorithm::TFRM::Setting::MAX_SHADOW_STEP_NUM);
	mainScenePass->GetMat()->GetShader()->SetFloat(Algorithm::TFRM::UNIFORM_NAME::MIN_SHADOW_STEP_NAME, Algorithm::TFRM::Setting::MIN_SHADOW_STEP_NUM);


	// 结果保存的相关设置
	METHOD_TYPE methodType_;
	int sceneType = SCENE_ID;
#if METHOD_ID == 0
	methodType_ = METHOD_TYPE::GroudTruth;
#elif METHOD_ID == 1
	methodType_ = METHOD_TYPE::ContrastMethod_T;
#elif METHOD_ID == 2
	methodType_ = METHOD_TYPE::ContrastMethod_F;
#else
	methodType_ = METHOD_TYPE::MyMethod;
#endif

	int maxImgSaveNum = 30;
	int imgIndex = 0;
	int frameIndex = 0;
	int maxFrameIndex = 500;
	Timer& timer = Timer::Instance();

#if COMPUTER_TIME == 1
	timer.Timing();
	std::string timeInfo_all = "";
#endif

	// 测试
	//Test();

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{

		// input
		// -----
		timer.Timing();
		INPUT::processInput(window);

#if COMPUTER_TIME == 1
		float t1 = 0.0, t2 = 0.0, t3 = 0.0, t4 = 0.0, t5 = 0.0, t6 = 0.0;
		std::string timeInfo = "Frame(" + std::to_string(frameIndex) + ") : ---CPU Solve Data Start---";
		timer.Timing();
#endif


		// 计算历史算法的因子
#if METHOD_ID == 1 || METHOD_ID == 3
		Algorithm::TFRM::ComputeTFRM(F_dis, F_ang, INPUT::inputCamera);
#endif

#if COMPUTER_TIME == 1
		t1 = timer.Timing();
		timeInfo = timeInfo + std::to_string(t1) + "(ms)" + "---Render Media Obj Start---";
#endif


		// render
		// ------
		mainScenePass->BindOutput();
		mainScenePass->GetMat()->GetShader()->SetInt("FrameIndex", frameIndex);
#if METHOD_ID == 1 || METHOD_ID == 3
		mainScenePass->GetMat()->GetShader()->SetFloat(Algorithm::TFRM::UNIFORM_NAME::FACTOR_DIS, F_dis);
		mainScenePass->GetMat()->GetShader()->SetFloat(Algorithm::TFRM::UNIFORM_NAME::FACTOR_ANG, F_ang);
#endif
		glClear(GL_DEPTH_BUFFER_BIT);
		mainScenePass->Draw();
		glFlush();

#if COMPUTER_TIME == 1
		t2 = timer.Timing();
		timeInfo = timeInfo + std::to_string(t2) + "(ms)";
#endif

#if (METHOD_ID == 2 || METHOD_ID == 3) && TEST_NO_BLUR_AND_FOVEAL == 0
		Foveated_pass_1->BindOutput();
		glClear(GL_DEPTH_BUFFER_BIT);
		Foveated_pass_1->Draw();

#if COMPUTER_TIME == 1
		t3 = timer.Timing();
		timeInfo = timeInfo + "---Foveated Pass 1 Start---" + std::to_string(t3) + "(ms)";
#endif
		
		Foveated_pass_2->BindOutput();
		glClear(GL_DEPTH_BUFFER_BIT);
		Foveated_pass_2->Draw();

#if COMPUTER_TIME == 1
		t4 = timer.Timing();
		timeInfo = timeInfo+ "---Foveated Pass 2 Start---" + std::to_string(t4) + "(ms)";
#endif

#if USE_GAUSS_BLUR == 1
		foveated_blur_pass_X->BindOutput();
		foveated_blur_pass_X->GetMat()->GetShader()->SetInt("blurDir", 1);
		glClear(GL_DEPTH_BUFFER_BIT);
		foveated_blur_pass_X->Draw();
		
		for(int i = 0;i < blurTimes;++i)
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

#if COMPUTER_TIME == 1
		t5 = timer.Timing();
		timeInfo = timeInfo + "---Blur Pass Start---" + std::to_string(t5) + "(ms)";
#endif

#endif

#endif

#if METHOD_ID == 0 || METHOD_ID == 1 || TEST_NO_BLUR_AND_FOVEAL == 1
		origin_show_pass->BindOutput(0);
		glClear(GL_DEPTH_BUFFER_BIT);
		origin_show_pass->Draw();
#endif 

#if COMPUTER_TIME == 1
		float t_all = t1 + t2 + t3 + t4 + t5 + t6;
		t_all = t_all < 0.001 ? 0.001 : t_all;
		timeInfo = timeInfo + "---End--- Total Cost Frame Time : " + std::to_string(t_all) + "(ms) / " + std::to_string(1.0 / t_all) + "(FPS) \n";
		timeInfo_all += timeInfo;
		std::cout << timeInfo << std::endl;
#endif

		// 更新相机的历史数据
		INPUT::inputCamera->UpdatePreMat();
		INPUT::inputCamera->UpdatePreAttr();


#if COMPUTER_QUALITY_ERROR == 1 && COMPUTER_TIME == 0
		// 保存结果
		BMPTool::Instance().GetScreenShot(methodType_, sceneType, imgIndex++);
#elif COMPUTER_QUALITY_ERROR == 2 && COMPUTER_TIME == 0
		BMPTool::Instance().GetQualityResult(sceneType, imgIndex++);
#endif

		frameIndex++;
		if (frameIndex > maxFrameIndex)
		{
			frameIndex = 0;
#if COMPUTER_TIME == 1
			timeInfo_all = "";
#endif
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

#if ONLY_RENDER_ONE_TURN == 0
		if (imgIndex > maxImgSaveNum) imgIndex = 0;
#elif ONLY_RENDER_ONE_TURN == 1 || COMPUTER_QUALITY_ERROR == 1
		if (imgIndex > maxImgSaveNum) break;
#endif

	}

#if COMPUTER_TIME == 1
	BMPTool::Instance().GetTimeResult(methodType_, sceneType, timeInfo_all);
#endif

	mainScenePass->GetMat()->GetShader()->vertexShader = "";
	mainScenePass->GetMat()->GetShader()->fragmentShader = "";
	frame->Delete();
	exit(0);
	//cloud_p_01.~shared_ptr();
	//Foveated_pass_1.~shared_ptr();
	//Foveated_pass_2.~shared_ptr();
	//
}


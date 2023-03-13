#include "TestScene.h"
#include "ShaderCompiler.h"
#include "EmbeddedData.h"
#include "VertexModel.h"
#include "Texture.h"
#include "Material.h"
#include "Actor.h"
#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Input.h"
#include "Config.h"
#include "Tools.h"
#include "Pass.h"
#include "FrameBuffer.h"
#include "UniformBuffer.h"
#include "Atmosphere.h"

using ShaderPtr = std::shared_ptr<ShaderCompiler>;
using TexturePtr = std::shared_ptr<Texture>;
using MaterialPtr = std::shared_ptr<Material>;
using ActorPtr = std::shared_ptr<Actor>;
using VertexModelPtr = std::shared_ptr<VertexModel>;
using FrameBufferPtr = std::shared_ptr<FrameBuffer>;
using LightPtr = std::shared_ptr<Light>;
using SkyPtr = std::shared_ptr<Atmosphere>;

Scene OPENGL_SCENE::TestScene::GetScene_Start01()
{
	ShaderCompiler baseTriangleShader(LEARN_OPENGL_SHADER::Light_02_lightTest_vs.c_str()
		, LEARN_OPENGL_SHADER::Light_02_lightTest_fs.c_str());
	baseTriangleShader.AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	baseTriangleShader.AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	baseTriangleShader.AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	baseTriangleShader.AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	baseTriangleShader.Compile();

	VertexModel baseTriangle;
	baseTriangle.BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);
	//baseTriangle.BindVertexToBuffer(LEARN_OPENGL::quadVertices, LEARN_OPENGL::quadIndices, LEARN_OPENGL::quadOffsets);

	Texture t1(0, LEARN_OPENGL_TEXTURE::START_01::Start_01_container.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name1, GL_REPEAT);
	Texture t2(1, LEARN_OPENGL_TEXTURE::START_01::Start_01_wall.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name2, GL_REPEAT);

	std::shared_ptr<Material> base_material = std::make_shared<Material>(std::make_shared<ShaderCompiler>(baseTriangleShader));
	base_material->AddTexture(std::make_shared<Texture>(t1));
	base_material->AddTexture(std::make_shared<Texture>(t2));
	base_material->LinkTextureForShader();

	std::shared_ptr<Actor> actor = std::make_shared<Actor>(std::make_shared<VertexModel>(baseTriangle), base_material);
	actor->SetPostionsArray(LEARN_OPENGL_POSITION::Start_01::cubePositions);

	std::shared_ptr<Actor> model1 = std::make_shared<Actor>(base_material);
	model1->loadModel(ASSETS::MODEL::MODEL_NANOSUIT);
	model1->ScaleTo(glm::vec3(0.05));

	Scene scene(INPUT::inputCamera);
	scene.AddActor(actor);
	scene.AddActor(model1);

	for (int i = 0; i < CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::POSITION_ARRAY.size(); ++i)
	{
		std::shared_ptr<PointLight> light = std::make_shared<PointLight>(CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::POSITION_ARRAY[i]);
		light->SetColor(CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::COLOR_ARRAY[i]);
		light->SetRadius(CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::RADIUS_ARRAY[i]);
		light->SetValue(100.0f);
		scene.AddLight(light);
	}

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_AlphaTest_02()
{
	Scene scene(INPUT::inputCamera);

	std::shared_ptr<ShaderCompiler> alphaTestShader= std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::High_03_AlphaTest_vs.c_str(), LEARN_OPENGL_SHADER::High_03_AlphaTest_fs.c_str());
	alphaTestShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	alphaTestShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	alphaTestShader->Compile();

	TexturePtr grass_tex = std::make_shared<Texture>(0, LEARN_OPENGL_TEXTURE::HIGH_03::High_03_Grass.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name1.c_str(), GL_CLAMP_TO_EDGE);
	TexturePtr container_tex = std::make_shared<Texture>(0, LEARN_OPENGL_TEXTURE::START_01::Start_01_container.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name1.c_str(), GL_REPEAT);
	TexturePtr wall_tex = std::make_shared<Texture>(0, LEARN_OPENGL_TEXTURE::START_01::Start_01_wall.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name1.c_str(), GL_REPEAT);

	MaterialPtr material = std::make_shared<Material>(alphaTestShader);
	material->AddTexture(grass_tex);
	material->LinkTextureForShader();
	MaterialPtr m1 = material->Copy();
	m1->AddTexture(container_tex);
	m1->LinkTextureForShader();
	MaterialPtr m2 = material->Copy();
	m2->AddTexture(wall_tex);
	m2->LinkTextureForShader();
	
	VertexModelPtr wall = std::make_shared<VertexModel>();
	wall->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_03::planeVertices, LEARN_OPENGL_VERTICE::HIGH_03::offsets);

	VertexModelPtr grass = std::make_shared<VertexModel>();
	grass->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_03::transparentVertices, LEARN_OPENGL_VERTICE::HIGH_03::offsets);

	VertexModelPtr box = std::make_shared<VertexModel>();
	box->BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);

	ActorPtr a1 = std::make_shared<Actor>(grass, material);
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};
	a1->SetPostionsArray(vegetation);

	
	ActorPtr a2 = std::make_shared<Actor>(box, m1);
	std::vector<glm::vec3> pos1 = { glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(2.0f, 0.0f, 0.0f) };
	a2->SetPostionsArray(pos1);
	
	ActorPtr a3 = std::make_shared<Actor>(wall, m2);

	scene.AddActor(a1);
	scene.AddActor(a2);
	scene.AddActor(a3);

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_AlphaBlend_03()
{
	//设置
	glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Scene scene(INPUT::inputCamera);

	std::shared_ptr<ShaderCompiler> alphaTestShader = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::High_03_AlphaTest_vs.c_str(), LEARN_OPENGL_SHADER::High_03_AlphaTest_fs.c_str());
	alphaTestShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	alphaTestShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	alphaTestShader->Compile();

	TexturePtr grass_tex = std::make_shared<Texture>(0, LEARN_OPENGL_TEXTURE::HIGH_03::High_03_window.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name1.c_str(), GL_CLAMP_TO_EDGE);
	TexturePtr container_tex = std::make_shared<Texture>(0, LEARN_OPENGL_TEXTURE::START_01::Start_01_container.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name1.c_str(), GL_REPEAT);
	TexturePtr wall_tex = std::make_shared<Texture>(0, LEARN_OPENGL_TEXTURE::START_01::Start_01_wall.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name1.c_str(), GL_REPEAT);

	MaterialPtr material = std::make_shared<Material>(alphaTestShader);
	material->AddTexture(grass_tex);
	material->LinkTextureForShader();
	MaterialPtr m1 = material->Copy();
	m1->AddTexture(container_tex);
	m1->LinkTextureForShader();
	MaterialPtr m2 = material->Copy();
	m2->AddTexture(wall_tex);
	m2->LinkTextureForShader();

	VertexModelPtr wall = std::make_shared<VertexModel>();
	wall->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_03::planeVertices, LEARN_OPENGL_VERTICE::HIGH_03::offsets);

	VertexModelPtr grass = std::make_shared<VertexModel>();
	grass->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_03::transparentVertices, LEARN_OPENGL_VERTICE::HIGH_03::offsets);

	VertexModelPtr box = std::make_shared<VertexModel>();
	box->BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);

	ActorPtr a1 = std::make_shared<Actor>(grass, material);
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};
	SortPosByDistance(vegetation, INPUT::inputCamera->Position);
	//std::cout << vegetation[0].r << std::endl;
	a1->SetPostionsArray(vegetation);


	ActorPtr a2 = std::make_shared<Actor>(box, m1);
	std::vector<glm::vec3> pos1 = { glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(2.0f, 0.0f, 0.0f) };
	a2->SetPostionsArray(pos1);

	ActorPtr a3 = std::make_shared<Actor>(wall, m2);

	scene.AddActor(a3);
	scene.AddActor(a2);
	scene.AddActor(a1);

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_FrameTest_04()
{
	Scene scene(INPUT::inputCamera);

	ShaderPtr postShader = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::High_04_FrameTest_vs.c_str(), LEARN_OPENGL_SHADER::High_04_FrameTest_fs.c_str());
	postShader->Compile();

	MaterialPtr postMat = std::make_shared<Material>(postShader);

	VertexModelPtr postObj = std::make_shared<VertexModel>();
	postObj->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_03::QuadVertices, LEARN_OPENGL_VERTICE::HIGH_03::QuadOffsets);

	ActorPtr actor = std::make_shared<Actor>(postObj, postMat);

	scene.AddActor(actor);

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_CubeMapTest_05()
{
	Scene scene(INPUT::inputCamera);

	ShaderPtr cubeMapShader = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::High_05_CubeMapTest_vs.c_str(), LEARN_OPENGL_SHADER::High_05_CubeMapTest_fs.c_str());
	cubeMapShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	cubeMapShader->Compile();

	ShaderPtr skyboxShader = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::High_06_SkyBox_vs.c_str(), LEARN_OPENGL_SHADER::High_06_SkyBox_fs.c_str());
	skyboxShader->Compile();

	TexturePtr skyTex = std::make_shared<CubeTexture>("skybox");
	skyTex->loadCubemap(LEARN_OPENGL_TEXTURE::HIGH_03::skybox1_face);

	VertexModelPtr cube = std::make_shared<VertexModel>();
	cube->BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);

	VertexModelPtr skybox = std::make_shared<VertexModel>();
	skybox->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_03::CubeMapVertice, LEARN_OPENGL_VERTICE::HIGH_03::CubeMapOffsets);

	MaterialPtr cubeMat = std::make_shared<Material>(cubeMapShader);
	cubeMat->AddTexture(skyTex);
	cubeMat->LinkTextureForShader();

	MaterialPtr skyMat = std::make_shared<Material>(skyboxShader);
	skyMat->AddTexture(skyTex);
	skyMat->LinkTextureForShader();
	skyMat->engineSetting->AddInitCmds(EngineCommands::Depth_Func_LEQUAL);
	skyMat->engineSetting->AddEndCmds(EngineCommands::Depth_Func_LESS);

	ActorPtr a1 = std::make_shared<Actor>(cube, cubeMat);
	ActorPtr a2 = std::make_shared<Actor>(skybox, skyMat);
	a2->_ActorType_().SetMobility(ActorType::Mobility::STATIC);

	scene.AddActor(a1);
	scene.AddActor(a2);

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_UniformBufferTest_06()
{
	Scene scene(INPUT::inputCamera);

	ShaderPtr sh = std::make_shared<ShaderCompiler>("shaders/UniformBufferTest_05.vs", "shaders/UniformBufferTest_05.fs");
	sh->Compile();
	sh->BlockBindingUniform(0, CONFIG::SHADER_DEFAULT_UNIFORM_NAME::UNIFORM_BLOCK_NAME::MATRIX_COORD_SYSTEM.c_str());


	std::shared_ptr<UniformBuffer> matrixBuffer=std::make_shared<UniformBuffer>(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::UNIFORM_BLOCK_NAME::MATRIX_COORD_SYSTEM);
	UniformData ud1(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX, UniformDataType::Mat4);
	matrixBuffer->AddData(ud1);
	UniformData ud2(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX, UniformDataType::Mat4);
	matrixBuffer->AddData(ud2);
	matrixBuffer->Create();
	matrixBuffer->LinkBindingPoint();
	auto proj = scene.MainCamera().GetProjectionMatrix();
	matrixBuffer->StoreData<glm::mat4>(proj, CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX);
	MaterialPtr m = std::make_shared<Material>(sh);

	VertexModelPtr cube = std::make_shared<VertexModel>();
	cube->BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);

	ActorPtr a = std::make_shared<Actor>(cube, m);

	scene.AddActor(a);
	scene.AddUniformBuffer(matrixBuffer);

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_GeometryNormal_07()
{
	Scene scene(INPUT::inputCamera);

	ShaderPtr sh = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::High_08_GeometryNormal_vs.c_str(), LEARN_OPENGL_SHADER::High_08_GeometryNormal_fs.c_str(), LEARN_OPENGL_SHADER::High_08_GeometryNormal_gs.c_str());
	sh->Compile();

	ShaderPtr baseTriangleShader= std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::Light_02_lightTest_vs.c_str()
		, LEARN_OPENGL_SHADER::Light_02_lightTest_fs.c_str());
	baseTriangleShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	baseTriangleShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	baseTriangleShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	baseTriangleShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	baseTriangleShader->Compile();

	MaterialPtr mat = std::make_shared<Material>(sh);
	MaterialPtr mat2 = std::make_shared<Material>(baseTriangleShader);

	ActorPtr man = std::make_shared<Actor>(mat);
	man->AddMaterial(mat2);
	man->loadModel(ASSETS::MODEL::MODEL_NANOSUIT);
	man->ScaleTo(glm::vec3(0.05));

	scene.AddActor(man);

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_InstanceTest_08()
{
	Scene scene(INPUT::inputCamera);
	
	ShaderPtr planetSh = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::High_09_InstanceTest_pla_vs.c_str(), LEARN_OPENGL_SHADER::High_09_InstanceTest_pla_fs.c_str());
	planetSh->Compile();

	ShaderPtr atoSh = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::High_09_InstanceTest_ast_vs.c_str(), LEARN_OPENGL_SHADER::High_09_InstanceTest_ast_fs.c_str());
	atoSh->Compile();

	MaterialPtr pltMat = std::make_shared<Material>(planetSh);
	MaterialPtr atoMat = std::make_shared<Material>(atoSh);

	ActorPtr plt = std::make_shared<Actor>(pltMat);
	plt->loadModel(ASSETS::MODEL::MODEL_PLANET);
	
	ActorPtr aco = std::make_shared<Actor>(atoMat);
	aco->loadModel(ASSETS::MODEL::MODEL_ASTERIOD);
	aco->SetInstanceNum(1000);

	auto data = MatTool::Instance().GenerateModelMat_R<1000>(10.0f, 5.0f);
	unsigned int offsets[4] = { 4, 4, 4, 4 };
	auto buffer = BufferTool::Instance().CreateArrayBuffer(data, 1000);

	auto func = [&buffer, &offsets](VertexModelPtr a)
	{
		a->AddAttribute(buffer, offsets);
	};

	std::for_each(aco->_Meshes_().begin(), aco->_Meshes_().end(), func);


	scene.AddActor(plt);
	scene.AddActor(aco);

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_ShadowMap_09()
{
	//glEnable(GL_CULL_FACE);
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	std::shared_ptr<Camera> light_camera = std::make_shared<Camera>();

	Scene scene(light_camera);

	ShaderPtr shadowMap_sh = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::HighLight_10_ShadowMapGet_vs.c_str(), LEARN_OPENGL_SHADER::HighLight_10_ShadowMapGet_fs.c_str());
	shadowMap_sh->Compile();


	MaterialPtr shadowMap_mat = std::make_shared<Material>(shadowMap_sh);
	//shadowMap_mat->engineSetting->AddInitCmds(EngineCommands::Cull_Enable);
	//shadowMap_mat->engineSetting->AddInitCmds(EngineCommands::Cull_Front);
	//shadowMap_mat->engineSetting->AddEndCmds(EngineCommands::Cull_Disable);

	VertexModelPtr cube = std::make_shared<VertexModel>();
	cube->BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);

	VertexModelPtr plane = std::make_shared<VertexModel>();
	plane->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_LIGHT_04::PlaneVertices, LEARN_OPENGL_VERTICE::HIGH_LIGHT_04::PlaneOffsets);

	ActorPtr a1 = std::make_shared<Actor>(cube, shadowMap_mat);
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 1.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 1.0f, -0.6f)
	};
	a1->SetPostionsArray(vegetation);
	ActorPtr a2 = std::make_shared<Actor>(plane, shadowMap_mat);

	scene.AddActor(a1);
	scene.AddActor(a2);

	scene.MainCamera().farPlane = 37.5f;
	scene.MainCamera().nearPlane = 1.0f;
	scene.MainCamera().Position = lightPos;
	scene.MainCamera().Front = -lightPos;
	scene.MainCamera().projectionType = ProjectionType::Ortho;

	CONFIG::SHADOW_MAP::DIR_LIGHT_CAMERA = scene.MainCamera();
	CONFIG::SHADOW_MAP::LIGHT_SPACE_MAT = CONFIG::SHADOW_MAP::DIR_LIGHT_CAMERA.GetProjectionMatrix() * CONFIG::SHADOW_MAP::DIR_LIGHT_CAMERA.GetViewMatrix();

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_BackFaceDepth_09_01()
{

	Scene scene(INPUT::inputCamera);

	ShaderPtr shadowMap_sh = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::HighLight_10_ShadowMapGet_vs.c_str(), LEARN_OPENGL_SHADER::HighLight_10_ShadowMapGet_fs.c_str());
	shadowMap_sh->Compile();


	MaterialPtr shadowMap_mat = std::make_shared<Material>(shadowMap_sh);
	shadowMap_mat->engineSetting->AddInitCmds(EngineCommands::Cull_Enable);
	shadowMap_mat->engineSetting->AddInitCmds(EngineCommands::Cull_Front);
	shadowMap_mat->engineSetting->AddEndCmds(EngineCommands::Cull_Disable);

	VertexModelPtr cube = std::make_shared<VertexModel>();
	cube->BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);

	ActorPtr a1 = std::make_shared<Actor>(cube, shadowMap_mat);
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 1.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 1.0f, -0.6f)
	};
	a1->SetPostionsArray(vegetation);

	scene.AddActor(a1);

//	CONFIG::SHADOW_MAP::DIR_LIGHT_CAMERA = scene.MainCamera();
//	CONFIG::SHADOW_MAP::LIGHT_SPACE_MAT = CONFIG::SHADOW_MAP::DIR_LIGHT_CAMERA.GetProjectionMatrix() * CONFIG::SHADOW_MAP::DIR_LIGHT_CAMERA.GetViewMatrix();

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_FrontFaceDepth_09_02()
{
	Scene scene(INPUT::inputCamera);

	ShaderPtr shadowMap_sh = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::HighLight_10_ShadowMapGet_vs.c_str(), LEARN_OPENGL_SHADER::HighLight_10_ShadowMapGet_fs.c_str());
	shadowMap_sh->Compile();


	MaterialPtr shadowMap_mat = std::make_shared<Material>(shadowMap_sh);
	
	VertexModelPtr cube = std::make_shared<VertexModel>();
	cube->BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);

	VertexModelPtr plane = std::make_shared<VertexModel>();
	plane->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_LIGHT_04::PlaneVertices, LEARN_OPENGL_VERTICE::HIGH_LIGHT_04::PlaneOffsets);


	ActorPtr a1 = std::make_shared<Actor>(cube, shadowMap_mat);
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 1.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 1.0f, -0.6f)
	};
	a1->SetPostionsArray(vegetation);

	ActorPtr a2 = std::make_shared<Actor>(plane, shadowMap_mat);

	scene.AddActor(a1);
	scene.AddActor(a2);

	//CONFIG::SHADOW_MAP::DIR_LIGHT_CAMERA = scene.MainCamera();
	//CONFIG::SHADOW_MAP::LIGHT_SPACE_MAT = CONFIG::SHADOW_MAP::DIR_LIGHT_CAMERA.GetProjectionMatrix() * CONFIG::SHADOW_MAP::DIR_LIGHT_CAMERA.GetViewMatrix();

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_ShadowDebug_10()
{

	Scene scene(INPUT::inputCamera);

	ShaderPtr debug_sh = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::HighLight_10_ShadowMapDebug_vs.c_str(), LEARN_OPENGL_SHADER::HighLight_10_ShadowMapDebug_fs.c_str());
	debug_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	debug_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	debug_sh->Compile();

	MaterialPtr debug_mat = std::make_shared<Material>(debug_sh);

	VertexModelPtr postObj = std::make_shared<VertexModel>();
	postObj->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_03::QuadVertices, LEARN_OPENGL_VERTICE::HIGH_03::QuadOffsets);

	ActorPtr a = std::make_shared<Actor>(postObj, debug_mat);

	scene.AddActor(a);

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_Cloud_11_01()
{
	glm::vec3 lightPos(0.2f, 0.01f, 1.0f);
	Scene scene(INPUT::inputCamera);

	ShaderPtr shadowMap_sh = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::HighLight_11_BaseShadow_vs.c_str(), LEARN_OPENGL_SHADER::HighLight_11_BaseShadow_fs.c_str());
	shadowMap_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	shadowMap_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::SHADOW);
	shadowMap_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	shadowMap_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	shadowMap_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	shadowMap_sh->Compile();

	ShaderPtr sky_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::SKY::earthSky_vs.c_str(), SHADER_PATH::RAY_MARCHING::SKY::earthSky_fs.c_str());
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	sky_sh->Compile();

	ShaderPtr Cloud_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::MEDIA::Cloud_01_vs.c_str(), SHADER_PATH::RAY_MARCHING::MEDIA::Cloud_01_fs.c_str());
	Cloud_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	Cloud_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	Cloud_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	Cloud_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	Cloud_sh->Compile();

	Texture t1(0, LEARN_OPENGL_TEXTURE::START_01::Start_01_container.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name1, GL_REPEAT);
	Texture t2(1, LEARN_OPENGL_TEXTURE::START_01::Start_01_wall.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name2, GL_REPEAT);
	Texture t3(0, ASSETS::TEXTURE::NOISY::Gray_Noise_Small.c_str(), std::string("GrayNoiseMap_S"), GL_REPEAT);
	t1.SetType("diffuseTexture");
	t2.SetType("diffuseTexture");



	MaterialPtr shadowMap_mat = std::make_shared<Material>(shadowMap_sh);
	shadowMap_mat->AddTexture(std::make_shared<Texture>(t2));
	shadowMap_mat->LinkTextureForShader();
	shadowMap_mat->GetShader()->SetMat4(CONFIG::SHADOW_MAP::SHADER_NAME::LIGHT_SPACE_MAT_NAME, CONFIG::SHADOW_MAP::LIGHT_SPACE_MAT);
	//shadowMap_mat->engineSetting->AddInitCmds(EngineCommands::Cull_Enable);
	//shadowMap_mat->engineSetting->AddInitCmds(EngineCommands::Cull_Front);
	//shadowMap_mat->engineSetting->AddEndCmds(EngineCommands::Cull_Disable);

	MaterialPtr sky_mat = std::make_shared<Material>(sky_sh);
	sky_mat->AddTexture(std::make_shared<Texture>(t3));
	sky_mat->LinkTextureForShader();
	sky_mat->engineSetting->AddInitCmds(EngineCommands::Depth_Func_LEQUAL);
	sky_mat->engineSetting->AddEndCmds(EngineCommands::Depth_Func_LESS);

	MaterialPtr Cloud_mat = std::make_shared<Material>(Cloud_sh);
	Cloud_mat->AddTexture(std::make_shared<Texture>(t3));
	Cloud_mat->LinkTextureForShader();
	Cloud_mat->GetShader()->SetMat4(CONFIG::SHADOW_MAP::SHADER_NAME::LIGHT_SPACE_MAT_NAME, CONFIG::SHADOW_MAP::LIGHT_SPACE_MAT);
	
	VertexModelPtr cube = std::make_shared<VertexModel>();
	cube->BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);
	//cube->BindVertexToBuffer(v_d, indices, offsets);
	//std::cout << v_d.size() << "  " << offsets.size();

	VertexModelPtr plane = std::make_shared<VertexModel>();
	plane->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_LIGHT_04::PlaneVertices, LEARN_OPENGL_VERTICE::HIGH_LIGHT_04::PlaneOffsets);

	ActorPtr a1 = std::make_shared<Actor>(cube, Cloud_mat);
	//a1->ScaleTo(glm::vec3(0.2));
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 1.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 1.0f, -0.6f)
	};
	a1->SetPostionsArray(vegetation);

	ActorPtr a2 = std::make_shared<Actor>(plane, shadowMap_mat);

	ActorPtr a3 = std::make_shared<Atmosphere>(sky_mat);
	a3->_ActorType_().SetMobility(ActorType::Mobility::STATIC);
	a3->LoadInfoToShader();

	scene.AddActor(a1);
	scene.AddActor(a2);
	scene.AddActor(a3);

	LightPtr dir1 = std::make_shared<DirectionalLight>(-lightPos);
	dir1->color = glm::vec3(1.0, 1.0, 0.8);
	dir1->value = 1.6;
	
	LightPtr point1 = std::make_shared<PointLight>(glm::vec3(-3.0, 2.0, 0.0));
	point1->SetColor(glm::vec3(1.0, 0.0, 0.0));
	point1->SetRadius(100.0);
	point1->SetValue(500.0);

	LightPtr point2 = std::make_shared<PointLight>(glm::vec3(3.0, 2.0, 0.0));
	point2->SetColor(glm::vec3(0.0, 0.0, 1.0));
	point2->SetRadius(60.0);
	point2->SetValue(800.0);

	LightPtr point3 = std::make_shared<PointLight>(glm::vec3(0.0, 2.0, 3.0));
	point3->SetColor(glm::vec3(0.0, 1.0, 0.0));
	point3->SetRadius(160.0);
	point3->SetValue(700.0);

	LightPtr point4 = std::make_shared<PointLight>(glm::vec3(0.0, 2.0, -3.0));
	point4->SetColor(glm::vec3(1.0, 1.0, 0.0));
	point4->SetRadius(40.0);
	point4->SetValue(600.0);

	//
	scene.AddLight(dir1);
	scene.AddLight(point1);
	scene.AddLight(point2);
	scene.AddLight(point3);
	scene.AddLight(point4);
	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_ShadowBase_11()
{
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
	Scene scene(INPUT::inputCamera);

	ShaderPtr shadowMap_sh = std::make_shared<ShaderCompiler>(LEARN_OPENGL_SHADER::HighLight_11_BaseShadow_vs.c_str(), LEARN_OPENGL_SHADER::HighLight_11_BaseShadow_fs.c_str());
	shadowMap_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::BRDF);
	shadowMap_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::SHADOW);
	shadowMap_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	shadowMap_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	shadowMap_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	shadowMap_sh->Compile();

	ShaderPtr sky_sh = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::SKY::earthSky_vs.c_str(), SHADER_PATH::RAY_MARCHING::SKY::earthSky_fs.c_str());
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	sky_sh->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	sky_sh->Compile();

	Texture t1(0, LEARN_OPENGL_TEXTURE::START_01::Start_01_container.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name1, GL_REPEAT);
	Texture t2(1, LEARN_OPENGL_TEXTURE::START_01::Start_01_wall.c_str(), LEARN_OPENGL_SHADER::TEXTURE_NAME::texture_name2, GL_REPEAT);
	Texture t3(0, ASSETS::TEXTURE::NOISY::Star.c_str(), std::string("noisyTexture"), GL_REPEAT);
	t1.SetType("diffuseTexture");
	t2.SetType("diffuseTexture");



	MaterialPtr shadowMap_mat = std::make_shared<Material>(shadowMap_sh);
	shadowMap_mat->AddTexture(std::make_shared<Texture>(t1));
	shadowMap_mat->LinkTextureForShader();
	shadowMap_mat->GetShader()->SetMat4(CONFIG::SHADOW_MAP::SHADER_NAME::LIGHT_SPACE_MAT_NAME, CONFIG::SHADOW_MAP::LIGHT_SPACE_MAT);
	

	MaterialPtr sky_mat = std::make_shared<Material>(sky_sh);
	sky_mat->AddTexture(std::make_shared<Texture>(t3));
	sky_mat->LinkTextureForShader();
	sky_mat->engineSetting->AddInitCmds(EngineCommands::Depth_Func_LEQUAL);
	sky_mat->engineSetting->AddEndCmds(EngineCommands::Depth_Func_LESS);


	VertexModelPtr cube = std::make_shared<VertexModel>();
	cube->BindVertexToBuffer(LEARN_OPENGL_VERTICE::START_01::Cube_vertices, LEARN_OPENGL_VERTICE::START_01::Cube_Offsets);
	//cube->BindVertexToBuffer(v_d, indices, offsets);
	//std::cout << v_d.size() << "  " << offsets.size();

	VertexModelPtr plane = std::make_shared<VertexModel>();
	plane->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_LIGHT_04::PlaneVertices, LEARN_OPENGL_VERTICE::HIGH_LIGHT_04::PlaneOffsets);

	ActorPtr a1 = std::make_shared<Actor>(cube, shadowMap_mat);
	//a1->ScaleTo(glm::vec3(0.2));
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 1.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 1.0f, -0.6f)
	};
	a1->SetPostionsArray(vegetation);

	auto mat2 = shadowMap_mat->Copy();
	mat2->textures.clear();
	mat2->AddTexture(std::make_shared<Texture>(t2));
	shadowMap_mat->LinkTextureForShader();
	ActorPtr a2 = std::make_shared<Actor>(plane, mat2);

	ActorPtr a3 = std::make_shared<Atmosphere>(sky_mat);
	a3->_ActorType_().SetMobility(ActorType::Mobility::STATIC);
	a3->LoadInfoToShader();

	scene.AddActor(a1);
	scene.AddActor(a2);
	scene.AddActor(a3);

	LightPtr dir1 = std::make_shared<DirectionalLight>(-lightPos);
	dir1->color = glm::vec3(1.0, 1.0, 0.8);
	dir1->value = 1.6;


	scene.AddLight(dir1);

	return scene;
}

Scene OPENGL_SCENE::TestScene::GetScene_FoveatedRender_12()
{
	Scene scene(INPUT::inputCamera);

	ShaderPtr postShader = std::make_shared<ShaderCompiler>(SHADER_PATH::POST_RENDER::FOVEA::KernelFovea_vs.c_str(), SHADER_PATH::POST_RENDER::FOVEA::KernelFovea_fs.c_str());
	postShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::POST_PROCESSING);
	postShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	postShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	postShader->Compile();

	postShader->UseSelf();
	glm::vec2 re = glm::vec2((float)CONFIG::SCREEN_CONFIG::FOVEA::FOVEA_WIDTH, (float)CONFIG::SCREEN_CONFIG::FOVEA::FOVEA_HEIGHT);
	postShader->SetVec2(CONFIG::MATERIAL_SETTINGS::FOVEA_RENDER::RESOLUTION_NAME, re);

	MaterialPtr postMat = std::make_shared<Material>(postShader);

	VertexModelPtr postObj = std::make_shared<VertexModel>();
	postObj->BindVertexToBuffer(LEARN_OPENGL_VERTICE::HIGH_03::QuadVertices, LEARN_OPENGL_VERTICE::HIGH_03::QuadOffsets);

	ActorPtr actor = std::make_shared<Actor>(postObj, postMat);

	scene.AddActor(actor);

	return scene;
}

Pass OPENGL_SCENE::TestPass::GetPass_FrameTest_04()
{
	Scene scene = OPENGL_SCENE::TestScene::Instance().GetScene_AlphaBlend_03();
	std::shared_ptr<FrameBuffer> frame = std::make_shared<FrameBuffer>(CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	frame->AddTexture(GL_RGB, "screenTexture", false);
	frame->NotifyGL();
	frame->AddRenderObject(true);

	Pass pass;
	pass.UpdateInput(std::make_shared<Scene>(scene));
	pass.UpdateOutput(frame);
	return pass;
}

Pass OPENGL_SCENE::TestPass::GetPass2_FrameTest_04(Pass& p)
{
	Scene scene2 = OPENGL_SCENE::TestScene::Instance().GetScene_FrameTest_04();
	Pass pass2;
	pass2.UpdateInput(std::make_shared<Scene>(scene2));
	pass2.UpdateGlobalMat(scene2.GetActorByIndex(0)->GetMaterial());
	pass2.GetMat()->AddTexture(p.GetOutput()->textureBuffers[0]);
	pass2.GetMat()->LinkTextureForShader();
	return pass2;
}

Pass OPENGL_SCENE::TestPass::GetPass_SkyBox_05()
{
	Scene scene = OPENGL_SCENE::TestScene::Instance().GetScene_CubeMapTest_05();

	Pass pass;
	pass.UpdateInput(std::make_shared<Scene>(scene));
	return pass;
}

Pass OPENGL_SCENE::TestPass::GetPass_UniformBufferTest_06()
{
	Scene scene = OPENGL_SCENE::TestScene::Instance().GetScene_UniformBufferTest_06();

	Pass pass;
	pass.UpdateInput(std::make_shared<Scene>(scene));
	return pass;
}

Pass OPENGL_SCENE::TestPass::GetPass_GeometryNormal_07()
{
	Scene scene = OPENGL_SCENE::TestScene::Instance().GetScene_GeometryNormal_07();

	Pass pass;
	pass.UpdateInput(std::make_shared<Scene>(scene));
	return pass;
}

Pass OPENGL_SCENE::TestPass::GetPass_InstancedTest_08()
{
	Scene scene = OPENGL_SCENE::TestScene::Instance().GetScene_InstanceTest_08();

	Pass pass;
	pass.UpdateInput(std::make_shared<Scene>(scene));
	return pass;
}

Pass OPENGL_SCENE::TestPass::GetPass_ShadowMap_09()
{
	Scene scene = OPENGL_SCENE::TestScene::Instance().GetScene_ShadowMap_09();
	FrameBufferPtr frame = std::make_shared<FrameBuffer>(CONFIG::SHADOW_MAP::SHADOW_WIDTH, CONFIG::SHADOW_MAP::SHADOW_HEIGHT);
	frame->AddTexture(GL_DEPTH_COMPONENT);


	Pass pass;
	pass.UpdateInput(std::make_shared<Scene>(scene));
	pass.UpdateOutput(frame);
	return pass;
}

Pass OPENGL_SCENE::TestPass::GetPass_BackFaceDepth_09_01()
{
	Scene scene = OPENGL_SCENE::TestScene::Instance().GetScene_BackFaceDepth_09_01();
	FrameBufferPtr frame = std::make_shared<FrameBuffer>(CONFIG::SHADOW_MAP::SHADOW_WIDTH, CONFIG::SHADOW_MAP::SHADOW_HEIGHT);
	frame->AddTexture(GL_DEPTH_COMPONENT);


	Pass pass;
	pass.UpdateInput(std::make_shared<Scene>(scene));
	pass.UpdateOutput(frame);
	return pass;
}

Pass OPENGL_SCENE::TestPass::GetPass_FrontFaceDepth_09_02()
{
	Scene scene = OPENGL_SCENE::TestScene::Instance().GetScene_FrontFaceDepth_09_02();
	FrameBufferPtr frame = std::make_shared<FrameBuffer>(CONFIG::SHADOW_MAP::SHADOW_WIDTH, CONFIG::SHADOW_MAP::SHADOW_HEIGHT);
	frame->AddTexture(GL_DEPTH_COMPONENT);


	Pass pass;
	pass.UpdateInput(std::make_shared<Scene>(scene));
	pass.UpdateOutput(frame);
	return pass;
}

Pass OPENGL_SCENE::TestPass::GetPass_ShadowDebug_10(Pass& p)
{
	Scene scene2 = OPENGL_SCENE::TestScene::Instance().GetScene_ShadowDebug_10();
	Pass pass2;
	pass2.UpdateInput(std::make_shared<Scene>(scene2));
	pass2.UpdateGlobalMat(scene2.GetActorByIndex(0)->GetMaterial());
	pass2.GetMat()->AddTexture(p.GetOutput()->textureBuffers[0]);
	pass2.GetMat()->LinkTextureForShader();
	return pass2;
}

Pass OPENGL_SCENE::TestPass::GetPass_BaseShadow_11(Pass& p)
{
	Scene scene2 = OPENGL_SCENE::TestScene::Instance().GetScene_ShadowBase_11();
	p.GetOutput()->textureBuffers[0]->SetType(CONFIG::SHADOW_MAP::SHADER_NAME::SHADOW_MAP_NAME);

	auto func = [&p](std::shared_ptr<Actor> a)
	{
		a->AddTextureForMaterial_A(p.GetOutput()->textureBuffers[0]);
	};

	std::for_each(scene2._Actors_().begin(), scene2._Actors_().end(), func);

	Pass pass2;
	pass2.UpdateInput(std::make_shared<Scene>(scene2));

	return pass2;
}

Pass OPENGL_SCENE::TestPass::GetPass_Cloud_11_01(Pass& shadow, Pass& depth)
{
	Scene scene2 = OPENGL_SCENE::TestScene::Instance().GetScene_Cloud_11_01();
	shadow.GetOutput()->textureBuffers[0]->SetType(CONFIG::SHADOW_MAP::SHADER_NAME::SHADOW_MAP_NAME);
	depth.GetOutput()->textureBuffers[0]->SetType(std::string("BackDepthMap"));

	auto func = [&shadow, &depth](std::shared_ptr<Actor> a)
	{
		a->AddTextureForMaterial_A(shadow.GetOutput()->textureBuffers[0]);
		a->AddTextureForMaterial_A(depth.GetOutput()->textureBuffers[0]);
	};

	std::for_each(scene2._Actors_().begin(), scene2._Actors_().end(), func);

	Pass pass2;
	pass2.UpdateInput(std::make_shared<Scene>(scene2));

	return pass2;
}

Pass OPENGL_SCENE::TestPass::GetPass_FoveatedRendering_12(Pass& p)
{
	std::shared_ptr<FrameBuffer> frame = std::make_shared<FrameBuffer>(CONFIG::SCREEN_CONFIG::FOVEA::FOVEA_WIDTH, CONFIG::SCREEN_CONFIG::FOVEA::FOVEA_HEIGHT);
	frame->AddTexture(GL_RGB, "screenTexture", false);
	frame->NotifyGL();
	frame->AddRenderObject(true);
	p.UpdateOutput(frame);

	Scene scene2 = TestScene::Instance().GetScene_FoveatedRender_12();


	Pass pass2;
	pass2.UpdateInput(std::make_shared<Scene>(scene2));
	pass2.UpdateGlobalMat(scene2.GetActorByIndex(0)->GetMaterial());
	pass2.GetMat()->AddTexture(p.GetOutput()->textureBuffers[0]);
	pass2.GetMat()->LinkTextureForShader();

	return pass2;
}

Pass OPENGL_SCENE::TestPass::GetPass_FoveatedRendering_Pass2_13(Pass& p)
{
	std::shared_ptr<FrameBuffer> frame = std::make_shared<FrameBuffer>(CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	frame->AddTexture(GL_RGB, "screenTexture", false);
	frame->NotifyGL();
	frame->AddRenderObject(true);
	p.UpdateOutput(frame);

	Scene scene2 = TestScene::Instance().GetScene_FoveatedRender_12();

	ShaderPtr postShader = std::make_shared<ShaderCompiler>(SHADER_PATH::POST_RENDER::FOVEA::KernelFovea_vs.c_str(), SHADER_PATH::POST_RENDER::FOVEA::KernelFovea_Pass2_fs.c_str());
	postShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::POST_PROCESSING);
	postShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	postShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	postShader->Compile();

	postShader->UseSelf();
	glm::vec2 re = glm::vec2((float)CONFIG::SCREEN_CONFIG::FOVEA::FOVEA_WIDTH, (float)CONFIG::SCREEN_CONFIG::FOVEA::FOVEA_HEIGHT);
	postShader->SetVec2(CONFIG::MATERIAL_SETTINGS::FOVEA_RENDER::RESOLUTION_NAME, re);

	MaterialPtr postMat = std::make_shared<Material>(postShader);

	Pass pass2;
	pass2.UpdateInput(std::make_shared<Scene>(scene2));
	pass2.UpdateGlobalMat(postMat);
	pass2.GetMat()->AddTexture(p.GetOutput()->textureBuffers[0]);
	pass2.GetMat()->LinkTextureForShader();
	
	return pass2;
}

Pass OPENGL_SCENE::TestPass::GetPass_RayMarchingRendering(Pass& main_tex, Pass& backDepth, Pass& frontDepth)
{
	//backDepth.GetOutput()->textureBuffers[0]->SetType(CONFIG::SHADOW_MAP::SHADER_NAME::SHADOW_MAP_NAME);
	frontDepth.GetOutput()->textureBuffers[0]->SetType(std::string("FrontDepthTex"));

	std::shared_ptr<FrameBuffer> frame = std::make_shared<FrameBuffer>(CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	frame->AddTexture(GL_RGB, "screenTexture", false);
	frame->NotifyGL();
	frame->AddRenderObject(true);
	main_tex.UpdateOutput(frame);

	Scene scene2 = TestScene::Instance().GetScene_FoveatedRender_12();

	ShaderPtr postShader = std::make_shared<ShaderCompiler>(SHADER_PATH::RAY_MARCHING::MEDIA::Cloud_02_vs.c_str(), 
		SHADER_PATH::RAY_MARCHING::MEDIA::Cloud_02_fs.c_str());
	postShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::RAY_MARCHING);
	postShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);
	postShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::MATH);
	postShader->AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::UNIFORM);
	postShader->Compile();

	MaterialPtr postMat = std::make_shared<Material>(postShader);
	Texture t3(0, ASSETS::TEXTURE::NOISY::Gray_Noise_Small.c_str(), std::string("GrayNoiseMap_S"), GL_REPEAT);

	Pass pass2;
	pass2.UpdateInput(std::make_shared<Scene>(scene2));
	pass2.UpdateGlobalMat(postMat);
	pass2.GetMat()->AddTexture(main_tex.GetOutput()->textureBuffers[0]);
	pass2.GetMat()->AddTexture(frontDepth.GetOutput()->textureBuffers[0]);
	pass2.GetMat()->AddTexture(std::make_shared<Texture>(t3));
	pass2.GetMat()->LinkTextureForShader();

	auto func = [&pass2](std::shared_ptr<Light>& a)
	{
		pass2.scene->AddLight(a);
	};

	std::for_each(main_tex.scene->_Lights_().begin(), main_tex.scene->_Lights_().end(), func);

	return pass2;
}

void OPENGL_SCENE::TestPass::Draw_FrameTest_04(Pass& p1, Pass& p2)
{
	p1.BindOutput();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	p1.Draw();

	p2.BindOutput();
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);
	p2.Draw();
}
void OPENGL_SCENE::TestPass::DrawShadowTest_05(Pass& p1, Pass& p2)
{
	glEnable(GL_CULL_FACE);
	//OPENGL_SCENE::TestPass::Intance().Draw_FrameTest_04(p1, p2);
	glViewport(0, 0, CONFIG::SHADOW_MAP::SHADOW_WIDTH, CONFIG::SHADOW_MAP::SHADOW_HEIGHT);
	p1.BindOutput();
	glClear(GL_DEPTH_BUFFER_BIT);
	p1.Draw();

	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//// reset viewport
	glViewport(0, 0, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	p2.Draw();
}
void OPENGL_SCENE::TestPass::DrawBaseShadow_06(Pass& p1, Pass& p2)
{
	//OPENGL_SCENE::TestPass::Intance().Draw_FrameTest_04(p1, p2);
	glViewport(0, 0, CONFIG::SHADOW_MAP::SHADOW_WIDTH, CONFIG::SHADOW_MAP::SHADOW_HEIGHT);
	p1.BindOutput();
	glClear(GL_DEPTH_BUFFER_BIT);
	p1.Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//// reset viewport
	glViewport(0, 0, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	p2.Draw();
}
void OPENGL_SCENE::TestPass::DrawFoveated_07(Pass& p1, Pass& p2, Pass& p3)
{


	//OPENGL_SCENE::TestPass::Intance().Draw_FrameTest_04(p1, p2);
	glViewport(0, 0, CONFIG::SHADOW_MAP::SHADOW_WIDTH, CONFIG::SHADOW_MAP::SHADOW_HEIGHT);
	p1.BindOutput();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	p1.Draw();
	


	//// reset viewport
	p2.BindOutput();
	glViewport(0, 0, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	p2.Draw();

	
	//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	p3.BindOutput();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_DEPTH_BUFFER_BIT);
	p3.Draw();
}
void OPENGL_SCENE::TestPass::DrawFoveated_Comp_08(Pass& p1, Pass& p2, Pass& p3, Pass& p4)
{
	//OPENGL_SCENE::TestPass::Intance().Draw_FrameTest_04(p1, p2);
	glViewport(0, 0, CONFIG::SHADOW_MAP::SHADOW_WIDTH, CONFIG::SHADOW_MAP::SHADOW_HEIGHT);
	p1.BindOutput();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	p1.Draw();


	//// reset viewport
	p2.BindOutput();
	glViewport(0, 0, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	p2.Draw();


	//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	

	p3.BindOutput();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_DEPTH_BUFFER_BIT);
	p3.Draw();

	p4.BindOutput();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_DEPTH_BUFFER_BIT);
	p4.Draw();
}
float angle = 10.0;
void OPENGL_SCENE::TestPass::DrawFoveated_Comp_09(Pass& p0, Pass& p1, Pass& p2, Pass& p3, Pass& p4)
{
	//glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, CONFIG::SHADOW_MAP::SHADOW_WIDTH, CONFIG::SHADOW_MAP::SHADOW_HEIGHT);
	p0.BindOutput();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	p0.Draw();

	//glDisable(GL_CULL_FACE);

	//OPENGL_SCENE::TestPass::Intance().Draw_FrameTest_04(p1, p2);
	p1.BindOutput();
	glViewport(0, 0, CONFIG::SHADOW_MAP::SHADOW_WIDTH, CONFIG::SHADOW_MAP::SHADOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	p1.Draw();


	//// reset viewport
	p2.BindOutput();
	glViewport(0, 0, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	p2.Draw();


	//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	for (auto i = p3.scene->_Lights_().begin() + 1; i != p3.scene->_Lights_().end(); ++i)
	{
		auto light = std::dynamic_pointer_cast<PointLight>(*i);
		auto pos = light->GetPos();
		auto mat = glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));
		pos = mat * glm::vec4(pos, 1.0);
		light->SetPos(pos);
	}
	/*auto sky = std::dynamic_pointer_cast<DirectionalLight>(p3.scene->_Lights_()[0]);
	auto dir = sky->GetDirection();
	if (dir.y < -3.3) angle = -angle;
	auto mat = glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(1.0, 0.0, 0.0));
	dir = mat * glm::vec4(dir, 1.0);
	sky->SetDirection(glm::vec3(dir));*/
	//glClearColor(0.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	p3.BindOutput();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	p3.Draw();

	
	//glEnable(GL_DEPTH_TEST);
	p4.BindOutput();
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	p4.Draw();
}
// 
// void OPENGL_SCENE::TestPass::DrawBaseShadow_06(Pass& p, Material& mat)
// {
// 	glViewport(0, 0, CONFIG::SHADOW_MAP::SHADOW_WIDTH, CONFIG::SHADOW_MAP::SHADOW_HEIGHT);
// 	p.BindOutput();
// 	glClear(GL_DEPTH_BUFFER_BIT);
// 	p.Draw();
// 
// 
// }

OPENGL_SCENE::PostPassFactory::PassPtr OPENGL_SCENE::PostPassFactory::CreateOne(OPENGL_SCENE::PostPassFactory::MaterialPtr mat)
{
	Scene scene = TestScene::Instance().GetScene_FoveatedRender_12();
	PassPtr p = std::make_shared<Pass>();
	p->UpdateGlobalMat(mat);
	p->UpdateInput(std::make_shared<Scene>(scene));
	//p->GetMat()->AddTexture(p->GetOutput()->textureBuffers[0]);
	//p->GetMat()->LinkTextureForShader();
	return p;
}

OPENGL_SCENE::PostPassFactory::PassPtr OPENGL_SCENE::PostPassFactory::CreateOne(OPENGL_SCENE::PostPassFactory::PassPtr pre_frame, OPENGL_SCENE::PostPassFactory::MaterialPtr mat)
{
	/* 让前一帧的输出绑定到这个frameBuffer */
	if (!pre_frame->GetOutput())
	{
		/* 让前一帧的输出绑定到这个frameBuffer */
		auto frame = std::make_shared<FrameBuffer>(CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
		frame->AddTexture(GL_RGB, "screenTexture", false);
		frame->NotifyGL();
		frame->AddRenderObject(true);
		pre_frame->UpdateOutput(frame);
	}

	Scene scene = TestScene::Instance().GetScene_FoveatedRender_12();
	Pass p;
	p.UpdateGlobalMat(mat);
	p.UpdateInput(std::make_shared<Scene>(scene));
	p.GetMat()->AddTexture(pre_frame->GetOutput()->textureBuffers[0]);
	p.GetMat()->LinkTextureForShader();

	/*for(int i=0;i< p.GetMat()->textures.size();i++)
		std::cout << p.GetMat()->textures[i]->GetType() << std::endl;*/

	return std::make_shared<Pass>(p);
}

OPENGL_SCENE::PostPassFactory::PassPtr OPENGL_SCENE::PostPassFactory::CreateOne(PassPtr pre_frame, MaterialPtr mat, float width, float height)
{
	/* 让前一帧的输出绑定到这个frameBuffer */
	if (!pre_frame->GetOutput())
	{
		/* 让前一帧的输出绑定到这个frameBuffer */
		auto frame = std::make_shared<FrameBuffer>(width, height);
		frame->AddTexture(GL_RGB, "screenTexture", false);
		frame->NotifyGL();
		frame->AddRenderObject(true);
		pre_frame->UpdateOutput(frame);
	}

	Scene scene = TestScene::Instance().GetScene_FoveatedRender_12();
	Pass p;
	p.UpdateGlobalMat(mat);
	p.UpdateInput(std::make_shared<Scene>(scene));
	p.GetMat()->AddTexture(pre_frame->GetOutput()->textureBuffers[0]);
	p.GetMat()->LinkTextureForShader();

	/*for(int i=0;i< p.GetMat()->textures.size();i++)
		std::cout << p.GetMat()->textures[i]->GetType() << std::endl;*/

	return std::make_shared<Pass>(p);
}

OPENGL_SCENE::PostPassFactory::PassPtr OPENGL_SCENE::PostPassFactory::CreateBackGround(OPENGL_SCENE::PostPassFactory::MaterialPtr mat)
{
	mat->Active();
	mat->SetMobility(false);
	mat->engineSetting->AddInitCmds(EngineCommands::Depth_Func_LEQUAL);
	mat->engineSetting->AddEndCmds(EngineCommands::Depth_Func_LESS);

	auto scene = CreateBackGround_Scene();
	Pass p;
	p.UpdateGlobalMat(mat);
	p.UpdateInput(scene);
	return std::make_shared<Pass>(p);
}

OPENGL_SCENE::PostPassFactory::PassPtr OPENGL_SCENE::PostPassFactory::CreateBackGround(OPENGL_SCENE::PostPassFactory::PassPtr pre_frame, OPENGL_SCENE::PostPassFactory::MaterialPtr mat)
{
	mat->Active();
	mat->SetMobility(false);
	mat->engineSetting->AddInitCmds(EngineCommands::Depth_Func_LEQUAL);
	mat->engineSetting->AddEndCmds(EngineCommands::Depth_Func_LESS);

	/* 让前一帧的输出绑定到这个frameBuffer */
	if (!pre_frame->GetOutput())
	{
		/* 让前一帧的输出绑定到这个frameBuffer */
		auto frame = std::make_shared<FrameBuffer>(CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
		frame->AddTexture(GL_RGB, "screenTexture", false);
		frame->NotifyGL();
		frame->AddRenderObject(true);
		pre_frame->UpdateOutput(frame);
	}

	auto scene = CreateBackGround_Scene();
	Pass p;
	p.UpdateGlobalMat(mat);
	p.UpdateInput(scene);
	p.GetMat()->AddTexture(pre_frame->GetOutput()->textureBuffers[0]);
	p.GetMat()->LinkTextureForShader();

	return std::make_shared<Pass>(p);
}

OPENGL_SCENE::PostPassFactory::ScenePtr OPENGL_SCENE::PostPassFactory::CreateBackGround_Scene()
{
	Scene scene(INPUT::inputCamera);
	ActorPtr a = std::make_shared<Atmosphere>();
	a->_ActorType_().SetMobility(ActorType::Mobility::STATIC);
	a->LoadInfoToShader();

	scene.AddActor(a);

	return std::make_shared<Scene>(scene);
}

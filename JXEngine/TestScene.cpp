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

using ShaderPtr = std::shared_ptr<ShaderCompiler>;
using TexturePtr = std::shared_ptr<Texture>;
using MaterialPtr = std::shared_ptr<Material>;
using ActorPtr = std::shared_ptr<Actor>;
using VertexModelPtr = std::shared_ptr<VertexModel>;

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
	//…Ë÷√
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
	std::cout << vegetation[0].r << std::endl;
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

Pass OPENGL_SCENE::TestPass::GetPass_FrameTest_04()
{
	Scene scene = OPENGL_SCENE::TestScene::Instance().GetScene_AlphaBlend_03();
	std::shared_ptr<FrameBuffer> frame = std::make_shared<FrameBuffer>(CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	frame->AddTexture(GL_RGB, "screenTexture");
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

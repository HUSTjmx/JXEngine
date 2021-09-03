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

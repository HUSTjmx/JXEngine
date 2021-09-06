#include <iostream>
#include "Scene.h"
#include "Actor.h"
#include "Camera.h"
#include "Light.h"
#include "EmbeddedData.h"
#include "Material.h"
#include "ShaderCompiler.h"
#include "Config.h"

Scene::Scene(std::shared_ptr<Camera> camera)
{
	this->camera = camera;
}

Scene& Scene::operator=(const Scene& scene)
{
	this->actors = scene.actors;
	this->camera = scene.camera;
	this->lights = scene.lights;
	return *this;
}

void Scene::AddActor(std::shared_ptr<Actor> a)
{
	actors.push_back(a);
}

void Scene::AddLight(std::shared_ptr<Light> l)
{
	lights.push_back(l);
}

void Scene::Draw()
{
	for (auto i = actors.begin(); i != actors.end();i++)
	{
		LoadLightInfo(i->get()->GetMaterial()->GetShader());
		LoadCameraInfo(i->get()->GetMaterial());
		i->get()->GetMaterial()->SetVP(*camera.get());
		i->get()->GetMaterial()->LoadInfoToShader();
		i->get()->Draw();
	}

	//ShaderCompiler lightCubeShader(LEARN_OPENGL_SHADER::Light_02_lightCube_vs.c_str(), LEARN_OPENGL_SHADER::Light_02_lightCube_fs.c_str());
	//lightCubeShader.Compile();
	////lightCubeShader.AddIncludeFile(CONFIG::SHADING_INCLUDE_CORE::LIGHT);

	//auto lightCube = actors[0];
	//std::shared_ptr<Material> lightCubeMaterial = std::make_shared<Material>(std::make_shared<ShaderCompiler>(lightCubeShader));

	//auto old_m = lightCube->GetMaterial();

	//lightCube->SetMaterial(lightCubeMaterial);
	//lightCube->SetPostionsArray(CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::POSITION_ARRAY);
	//lightCube->GetMaterial()->SetVP(*camera.get());
	//lightCube->AnimaTo(glm::vec3(0.5), glm::vec3(0.25), 30.0f);
	//lightCube->Draw();
	//lightCube->SetMaterial(old_m);
	
}

void Scene::Draw(std::shared_ptr<Material> newMat)
{
	for (auto i = actors.begin(); i != actors.end();i++)
	{
		LoadLightInfo(newMat->GetShader());
		LoadCameraInfo(newMat);
		newMat->SetVP(*camera.get());
		newMat->LoadInfoToShader();
		i->get()->Draw(newMat);
	}
}

void Scene::Delete()
{
	for (auto i = actors.begin(); i != actors.end();i++)
		i->get()->Delete();
}

Camera& Scene::MainCamera()
{
	// TODO: 在此处插入 return 语句
	return *camera.get();
}

std::shared_ptr<Actor> Scene::GetActorByIndex(int index)
{
	return actors[index];
}

void Scene::LoadLightInfo(std::shared_ptr<ShaderCompiler> shader)
{
	shader->SetInt(CONFIG::LIGHT_SETTINGS::SHADER_DIR_LIGHT::NUM_STRING, DirectionalLight::ObjectCount());
	shader->SetInt(CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::NUM_STRING, PointLight::ObjectCount());
	shader->SetInt(CONFIG::LIGHT_SETTINGS::SHADER_SPOT_LIGHT::NUM_STRING, SpotLight::ObjectCount());
	//std::cout << DirectionalLight::ObjectCount() << PointLight::ObjectCount() << SpotLight::ObjectCount() << std::endl;
	for (auto i = lights.begin(); i != lights.end(); ++i)
		i->get()->LoadInfoToShader(shader);
}

void Scene::LoadCameraInfo(std::shared_ptr<Material> material)
{
	material->SetViewPos(*camera.get());
}

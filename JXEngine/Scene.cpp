#include <iostream>
#include "Scene.h"
#include "Actor.h"
#include "Camera.h"
#include "Light.h"
#include "EmbeddedData.h"
#include "Material.h"
#include "ShaderCompiler.h"
#include "UniformBuffer.h"
#include "Config.h"
#include "Timer.h"

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

void Scene::AddUniformBuffer(std::shared_ptr<UniformBuffer> ub)
{
	uniformBuffers.push_back(ub);
}

void Scene::Draw()
{
	auto coord_mat_buffer = FindUniformBuffer(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::UNIFORM_BLOCK_NAME::MATRIX_COORD_SYSTEM);

	if (coord_mat_buffer != -1)
	{
		auto view = camera->GetViewMatrix();
		uniformBuffers[coord_mat_buffer]->StoreData<glm::mat4>(view, CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX);
	}

	for (auto i = actors.begin(); i != actors.end();i++)
	{
		auto func = [this](std::shared_ptr<Material> a) {
			this->LoadLightInfo(a);
			this->LoadCameraInfo(a);
			this->LoadDefaultInfo(a);
		};
		std::for_each(i->get()->_Materials_().begin(), i->get()->_Materials_().end(), func);

		if (coord_mat_buffer == -1)
		{
			auto func2 = [this](std::shared_ptr<Material> a)
			{
				a->SetVP(*camera.get());
			};
			std::for_each(i->get()->_Materials_().begin(), i->get()->_Materials_().end(), func2);
		}
		//i->get()->LoadInfoToShader();
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
	auto coord_mat_buffer = FindUniformBuffer(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::UNIFORM_BLOCK_NAME::MATRIX_COORD_SYSTEM);

	if (coord_mat_buffer != -1)
	{
		auto view = camera->GetViewMatrix();
		uniformBuffers[coord_mat_buffer]->StoreData<glm::mat4>(view, CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX);
	}

	for (auto i = actors.begin(); i != actors.end();i++)
	{
		LoadLightInfo(newMat);
		LoadCameraInfo(newMat);
		LoadDefaultInfo(newMat);
		if (coord_mat_buffer == -1)
			newMat->SetVP(*camera.get());
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

void Scene::LoadLightInfo(std::shared_ptr<Material> material)
{
	material->Active();
	//std::cout << material->GetShader()->ID << std::endl;
	material->GetShader()->SetInt(CONFIG::LIGHT_SETTINGS::SHADER_DIR_LIGHT::NUM_STRING, DirectionalLight::ObjectCount());
	material->GetShader()->SetInt(CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::NUM_STRING, PointLight::ObjectCount());
	material->GetShader()->SetInt(CONFIG::LIGHT_SETTINGS::SHADER_SPOT_LIGHT::NUM_STRING, SpotLight::ObjectCount());
	
	//std::cout << DirectionalLight::ObjectCount() << PointLight::ObjectCount() << SpotLight::ObjectCount() << std::endl;
	for (auto i = lights.begin(); i != lights.end(); ++i)
		i->get()->LoadInfoToShader(material->GetShader());

	if (DirectionalLight::ObjectCount() > 0)
	{
		material->GetShader()->SetMat4(CONFIG::SHADOW_MAP::SHADER_NAME::LIGHT_SPACE_MAT_NAME, CONFIG::SHADOW_MAP::LIGHT_SPACE_MAT);
	}
}

void Scene::LoadCameraInfo(std::shared_ptr<Material> material)
{
	camera->LoadInfoToShader(material);
}

void Scene::LoadDefaultInfo(std::shared_ptr<Material> material)
{
	glm::vec2 resolution = glm::vec2((float)CONFIG::SCREEN_CONFIG::SCR_WIDTH, (float)CONFIG::SCREEN_CONFIG::SCR_HEIGHT);

	material->Active();
	//std::cout << Timer::Instance().GetCurrentTime() << std::endl;
	material->GetShader()->SetFloat(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::TIME, Timer::Instance().GetCurrentTime());
	material->GetShader()->SetVec2(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::RESOLUTION, resolution);
}

int Scene::FindUniformBuffer(const std::string& name)
{
	int i = -1;
	int b = 0;
	auto func = [&i, name, &b](std::shared_ptr<UniformBuffer> s) {
		if (std::strcmp(s->GetName().c_str(), name.c_str()) == 0) i = b;
		b++;
	};
	
	std::for_each(uniformBuffers.begin(), uniformBuffers.end(), func);
	//std::cout << i << std::endl;
	return i;
}

std::vector<std::shared_ptr<Actor>>& Scene::_Actors_()
{
	// TODO: 在此处插入 return 语句
	return actors;
}

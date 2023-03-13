#include <algorithm>
#include <type_traits>
#include "Material.h"
#include "Texture.h"
#include "ShaderCompiler.h"
#include <iostream>
#include <vector>
#include "Camera.h"
#include "Config.h"
#include "Actor.h"
#include "Tools.h"

Material::Material()
{
	baseColor = glm::vec3(1.0);

	metallic = 0.0;

	roughness = 0.9;

	reflectance = 0.1;

	emissive = glm::vec3(0.0);

	CanMove = true;

	IsJitter = false;

	engineSetting = std::make_shared<EngineSetting>();
}

Material::Material(std::shared_ptr<ShaderCompiler> s)
{
	UpdateShader(s);

	baseColor = glm::vec3(1.0);

	metallic = 0.0;

	roughness = 0.9;

	reflectance = 0.1;

	emissive = glm::vec3(0.0);

	CanMove = true;

	IsJitter = false;

	engineSetting = std::make_shared<EngineSetting>();
}

void Material::OnNotify(Event* event)
{
	auto type_ = dynamic_cast<ActorType*>(event);
	CanMove = type_->GetMobility() == ActorType::Mobility::MOVE;
	std::cout << CanMove << std::endl;
}


void Material::AddTexture(std::shared_ptr<Texture> tex)
{
	textures.push_back(tex);
}

void Material::UpdateShader(std::shared_ptr<ShaderCompiler> s)
{
	shader = s;
}

void Material::BindTexturesToOpenGL()
{
	for (auto i = textures.begin(); i != textures.end();i++)
	{
		i->get()->SetUniformPos(i - textures.begin());
		i->get()->SetForShader(*shader);
		i->get()->Bind();
	}
}

void Material::LinkTextureForShader()
{	
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (auto i = textures.begin(); i != textures.end();i++)
	{
		std::string number = "";
		std::string name = i->get()->GetType();
		if (name == "texture_albedo")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream

		//i->get()->SetUniformPos(i - textures.begin());
		i->get()->SetType(name + number);
		std::cout << name + number << " "<< std::endl;
		//i->get()->SetForShader(*shader);
		
		
	}
}

void Material::Active() const
{
	shader->UseSelf();
}

void Material::LoadInfoToShader() const
{
	Active();
	shader->SetVec3(CONFIG::MATERIAL_SETTINGS::STANDORD_MODEL::BASE_COLOR, baseColor);
	shader->SetFloat(CONFIG::MATERIAL_SETTINGS::STANDORD_MODEL::METALLIC, metallic);
	shader->SetFloat(CONFIG::MATERIAL_SETTINGS::STANDORD_MODEL::ROUGHNESS, roughness);
	shader->SetFloat(CONFIG::MATERIAL_SETTINGS::STANDORD_MODEL::REFLECTANCE, reflectance);
	shader->SetVec3(CONFIG::MATERIAL_SETTINGS::STANDORD_MODEL::EMISSIVE, emissive);
}

std::shared_ptr<ShaderCompiler> Material::GetShader()
{
	return shader;
}

void Material::SetMVP(const Camera& camera, Transform transform)
{
	Active();
	GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::MODEL_MATRIX, transform.model);
	GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX, camera.GetProjectionMatrix());
	if (CanMove)
		GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX, camera.GetViewMatrix());
	else
	{
		GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX, MatTool::Instance().RemoveTranslation(camera.GetViewMatrix()));
	}
}

void Material::SetVP(const Camera& camera)
{
	Active();
	auto project_mat = camera.GetProjectionMatrix();
	auto view = camera.GetViewMatrix();
	if (IsJitter)
	{
		project_mat = MatTool::Instance().JitterMat<16>(project_mat, Counter<0>::times, CONFIG::SCREEN_CONFIG::SCR_WIDTH * 2, CONFIG::SCREEN_CONFIG::SCR_HEIGHT * 2);
	}

	GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX, project_mat);
	GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX_INV, glm::inverse(project_mat));
	GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::PROJECTION_MATRIX_PRE, camera.Pre_ProjMat);
	//std::cout << CanMove << std::endl;
	if (!CanMove)
	{
		//std::cout << "xxx" << std::endl;
		view = MatTool::Instance().RemoveTranslation(view);
	}
	GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX, view);
	GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX_INV, glm::inverse(view));
	GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::VIEW_MATRIX_PRE, camera.Pre_ViewMat);
}

void Material::SetP(Transform transform)
{
	Active();

	GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::MODEL_MATRIX, transform.model);
}

void Material::SetViewPos(const Camera& camera)
{
	Active();
	GetShader()->SetVec3(CONFIG::CAMERA_CONFIG::SHADER::VIEW_POS_IN_WORLD, camera.Position);
}

void Material::SetMobility(bool movbility)
{
	CanMove = movbility;
}

void Material::SetJitter(bool isJitter)
{
	IsJitter = isJitter;
}

std::vector<std::shared_ptr<Texture>> Material::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string directory)
{
	std::vector<std::shared_ptr<Texture>> textures_t;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures.size(); j++)
		{
			if (std::strcmp(textures[j]->GetPath().data(), str.C_Str()) == 0)
			{
				textures_t.push_back(textures[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture(str.C_Str(), directory, false, GL_REPEAT);
			texture.SetType(typeName);
			texture.SetPath(str.C_Str());
			textures_t.push_back(std::make_shared<Texture>(texture));
			textures.push_back(std::make_shared<Texture>(texture));  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures_t;
}

void Material::LoadTextures(aiMaterial* mat,std::string directory)
{
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	std::vector<std::shared_ptr<Texture>> diffuseMaps = LoadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse", directory);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<std::shared_ptr<Texture>> specularMaps = LoadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular", directory);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<std::shared_ptr<Texture>> normalMaps = LoadMaterialTextures(mat, aiTextureType_HEIGHT, "texture_normal", directory);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<std::shared_ptr<Texture>> heightMaps = LoadMaterialTextures(mat, aiTextureType_AMBIENT, "texture_height", directory);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
}

std::shared_ptr<Material> Material::Copy() const
{
	std::shared_ptr<ShaderCompiler> shader2 = std::make_shared<ShaderCompiler>();
	shader->Copy(*shader2.get());

	std::shared_ptr<Material> m = std::make_shared<Material>(shader2);
	m->baseColor = baseColor;
	m->emissive = emissive;
	m->metallic = metallic;
	m->reflectance = reflectance;
	m->roughness = roughness;
	//m->engineSetting = engineSetting;
	//m->textures = textures;
	return m;
}

std::shared_ptr<Material> Material::CopyWithTex()
{
	std::shared_ptr<ShaderCompiler> shader2 = std::make_shared<ShaderCompiler>();
	shader->Copy(*shader2.get());

	std::shared_ptr<Material> m = std::make_shared<Material>(shader2);
	m->baseColor = baseColor;
	m->emissive = emissive;
	m->metallic = metallic;
	m->reflectance = reflectance;
	m->roughness = roughness;
	//m->engineSetting = engineSetting;
	m->textures = textures;
	return m;
}


void TranslateCmd(EngineCommands cmd)
{
	switch (cmd)
	{
	case EngineCommands::Depth_Func_LEQUAL:
		glDepthFunc(GL_LEQUAL);
		break;
	case EngineCommands::Depth_Func_LESS:
		glDepthFunc(GL_LESS);
		break;
	case  EngineCommands::Depth_Clear:
		glClear(GL_DEPTH_BUFFER_BIT);
		break;
	case EngineCommands::Cull_Front:
		glCullFace(GL_FRONT);
		break;
	case EngineCommands::Cull_Back:
		glCullFace(GL_BACK);
		break;
	case EngineCommands::Cull_Enable:
		glEnable(GL_CULL_FACE);
		break;
	case EngineCommands::Cull_Disable:
		glDisable(GL_CULL_FACE);
		break;
	default:
		break;
	}
}

void EngineSetting::AddInitCmds(EngineCommands cmd)
{
	initCmds.push_back(cmd);
}

void EngineSetting::AddEndCmds(EngineCommands cmd)
{
	endCmds.push_back(cmd);
}

void EngineSetting::InitExecutive() const
{
	std::for_each(initCmds.begin(), initCmds.end(), TranslateCmd);
}

void EngineSetting::EndExecutive() const
{
	std::for_each(endCmds.begin(), endCmds.end(), TranslateCmd);
}

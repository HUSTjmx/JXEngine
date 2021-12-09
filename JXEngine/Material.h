// Copyright 2021.	All rights reserved
//
// Author: 2568094892@qq.com (Jiang Meng Xian)
// ...
#pragma once
#include <memory>
#include <vector>
#include <string>
#include <typeinfo>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "DesignPatterns.h"

class Texture;
class ShaderCompiler;
class Camera;
class Transform;


// Uniform data type.
// ...
enum class UNIFORM_DATA_TYPE {
	Int,
	Float,
	Bool,
	Vec2,
	Vec3,
	Vec4,
	Mat2,
	Mat3,
	Mat4
};

// Engine Commands, a enum element can represent a opengl command.
// ...
enum class EngineCommands
{
	Depth_Func_LEQUAL,
	Depth_Func_LESS,
	Depth_Clear,
	Cull_Front,
	Cull_Back,
	Cull_Enable,
	Cull_Disable
};

// Engine commands obj.
// ...
class EngineSetting
{
public:
	EngineSetting() {}

	// Add a init command before this material is drawed.
	// ...
	void AddInitCmds(EngineCommands cmd);

	// Add a end command after this material is drawed.
	// ...
	void AddEndCmds(EngineCommands cmd);

	// Call init commands.
	// ...
	void InitExecutive() const;

	// Call end commands.
	// ...
	void EndExecutive() const;

private:

	// The array of init commands
	// ...
	std::vector<EngineCommands> initCmds;

	// The array of end commands
	// ...
	std::vector<EngineCommands> endCmds;
};

// The material obj, one of the main class.
// ...
class Material : public Observer
{

public:

	Material(std::shared_ptr<ShaderCompiler> s);

	// The design pattern, observer pattern.
	// The observer, that is the listener.
	// ...
	void OnNotify(Event* event)override;

	// Create a new different obj with the same setting.
	// ...

	// Add a texture for the material.
	// ...
	virtual void AddTexture(std::shared_ptr<Texture> tex);

	// Update the shader code of the material.
	// ...
	virtual void UpdateShader(std::shared_ptr<ShaderCompiler> s);

	// The function is used when we have finished setting of the material. But in reality, we maybe delete it.
	// It may is usefulness.
	// ...
	virtual void BindTexturesToOpenGL();

	// On the fly ,we should bind texture for the material.
	// ...
	virtual void LinkTextureForShader();

	// Active the material that the pass is draw.
	virtual void Active() const;

	// Load some info to shader, in a word, we set uniform data.
	// ...
	virtual void LoadInfoToShader()const;

	// ...
	virtual std::shared_ptr<ShaderCompiler> GetShader();

	// Set MVP mat.
	// Note : We maybe need change this pos of update MVP to other class.
	// ...
	void SetMVP(const Camera& camera, Transform transform);

	// Set VP mat.
	// Note : We maybe need change this pos of update VP to other class.
	// ...
	void SetVP(const Camera& camera);

	// Set P mat.
	// Note : We maybe need change this pos of update P to other class.
	// ...
	void SetP(Transform transform);

	// Set view pos.
	// ...
	void SetViewPos(const Camera& camera);

	// Set the mobility that show we should delete the translation of View or not.
	// ...
	void SetMobility(bool movbility);

	// ...
	void SetJitter(bool isJitter);

	std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string directory);

	void LoadTextures(aiMaterial* mat, std::string directory);

	template<typename T>
	void SetUniformAttri(const std::string& name, T value, void(*pf)(const std::string&, T));

	std::shared_ptr<Material> Copy() const;

	std::vector<std::shared_ptr<Texture>> textures;

	std::shared_ptr<EngineSetting> engineSetting;

private:

	std::shared_ptr<ShaderCompiler> shader;

	glm::vec3 baseColor;
	
	float metallic;

	float roughness;

	float reflectance;

	glm::vec3 emissive;

	bool CanMove;

	// Is jitter the proj mat?
	// ...
	bool IsJitter;
};

template<typename T>
inline void Material::SetUniformAttri(const std::string& name, T value, void(*pf)(const std::string&, T))
{

	//if (typeid(value) == typeid(1))
	//{
	//	//shader->SetInt(name, value);
	//	//shader->SetUniform<T>(name, value, &ShaderCompiler::SetInt);
	//	pf(name, value);
	//}
		//shader->SetUniform<T>(name, std::forward<T>(value), &ShaderCompiler::SetInt);
		//;

	/*else if (typeid(value) == typeid(1.0f))
		shader->SetUniform(&ShaderCompiler::SetFloat, std::forward<T>(value), name);

	else if (typeid(value) == typeid(true))
		shader->SetUniform(&ShaderCompiler::SetBool, std::forward<T>(value), name);

	else if (typeid(value) == typeid(glm::mat4(1.0)))
		shader->SetUniform(&ShaderCompiler::SetMat4, std::forward<T>(value), name);*/

	/*switch (type_)
	{
	case typeid(1):
		shader->SetInt(name, value);
		break;
	case typeid(1.0f):
		shader->SetFloat(name, value);
		break;
	case typeid(true):
		shader->SetBool(name, value);
		break;
	case typeid(glm::vec2(1.0)):
		shader->SetVec2(name, value);
		break;
	case typeid(glm::vec3(1.0)):
		shader->SetVec3(name, value);
		break;
	case typeid(glm::vec4(1.0)):
		shader->SetVec4(name, value);
		break;
	case typeid(glm::mat2(1.0)):
		shader->SetMat2(name, value);
		break;
	case typeid(glm::mat3(1.0)):
		shader->SetMat3(name, value);
		break;
	case typeid(glm::mat4(1.0)):
		shader->SetMat4(name, value);
		break;
	default:
		break;
	}*/
}

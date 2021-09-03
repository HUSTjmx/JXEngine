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


class Texture;
class ShaderCompiler;
class Camera;
class Transform;

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

class Material
{

public:

	Material(std::shared_ptr<ShaderCompiler> s);

	virtual void AddTexture(std::shared_ptr<Texture> tex);

	virtual void UpdateShader(std::shared_ptr<ShaderCompiler> s);

	virtual void BindTexturesToOpenGL();

	virtual void LinkTextureForShader();

	virtual void Active() const;

	virtual void LoadInfoToShader()const;

	virtual std::shared_ptr<ShaderCompiler> GetShader();

	void SetMVP(const Camera& camera, Transform transform);

	void SetVP(const Camera& camera);

	void SetP(Transform transform);

	void SetViewPos(const Camera& camera);

	std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string directory);

	void LoadTextures(aiMaterial* mat, std::string directory);

	template<typename T>
	void SetUniformAttri(const std::string& name, T value, void(*pf)(const std::string&, T));


	std::shared_ptr<Material> Copy() const;

	std::vector<std::shared_ptr<Texture>> textures;


private:

	std::shared_ptr<ShaderCompiler> shader;

	glm::vec3 baseColor;
	
	float metallic;

	float roughness;

	float reflectance;

	glm::vec3 emissive;
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

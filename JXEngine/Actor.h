#pragma once
#include "std.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "DesignPatterns.h"

class Material;
class VertexModel;
class Camera;
class Texture;
class ShaderCompiler;

struct Transform {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation_axis;
	float angle;
	glm::mat4 model;

	Transform() : position(0.0, 0.0, 0.0), scale(1.0, 1.0, 1.0), rotation_axis(0.0, 1.0, 0.0), angle(0.0f), model(glm::mat4(1.0f)) {}

	void Move(glm::vec3 delta);

	void MoveTo(glm::vec3 newPosition);

    void Scale(glm::vec3 delta);

	void ScaleTo(glm::vec3 newScale);

	void Rotate(glm::vec3 Axis, float angle);

	void Reset();

};

class ActorType : public Event, public Subject
{
public:

	enum class Mobility {
		STATIC,
		MOVE
	};

	ActorType() : mobility(Mobility::MOVE) {}

	void SetMobility(Mobility a);

	Mobility GetMobility();

private:

	Mobility mobility;
};

class Actor
{
public:
	Actor() {}

	Actor(std::shared_ptr<Material> m);

	Actor(const std::string& path, std::shared_ptr<Material> m);

	Actor(std::shared_ptr<VertexModel> m, std::shared_ptr<Material> n);

	Actor(const Actor& a) : meshes(a.meshes), materials(a.materials), transform(a.transform), positions(a.positions) {}

	Actor& operator=(const Actor& a);

	virtual void AddMesh(std::shared_ptr<VertexModel> m);

	virtual void AddMaterial(std::shared_ptr<Material>);

	virtual void SetPostionsArray(std::vector<glm::vec3>& p);

	virtual std::shared_ptr<Material> GetMaterial(int index = 0);

	// Load some info about self to shader.
	// ...
	virtual void LoadInfoToShader() {}

	virtual void Draw();

	virtual void Draw(std::shared_ptr<Material> newMat);

	virtual void Delete();

	virtual void Anima(glm::vec3 pos_delta, glm::vec3 sca_delta = glm::vec3(0.0), float angle = 0.0f, glm::vec3 axis = glm::vec3(0.0, 1.0, 0.0));
	
	virtual void AnimaTo(glm::vec3 pos, glm::vec3 scale = glm::vec3(1.0), float angle = 0.0f, glm::vec3 axis = glm::vec3(0.0, 1.0, 0.0));

	virtual void ScaleTo(glm::vec3 scale);

	void loadModel(std::string const& path);

	void processNode(aiNode* node, const aiScene* scene);

	std::shared_ptr<VertexModel> processMesh(aiMesh* mesh, const aiScene* scene);

	// Setup the number of instance draw
	// ...
	void SetInstanceNum(unsigned int num);

	ActorType& _ActorType_() {
		return type;
	}

	Transform& _Transform_()
	{
		return transform;
	}

	// Return mesh array.
	// Reference transport.
	// ...
	std::vector<std::shared_ptr<VertexModel>>& _Meshes_();

	// Return materials array.
	// Reference transport.
	// ...
	std::vector<std::shared_ptr<Material>>& _Materials_();

	// Add a texture for all material, e.g. shadowMap.
	// ...
	void AddTextureForMaterial_A(std::shared_ptr<Texture> tex);

private:

	void InitPosition();

	std::vector<std::shared_ptr<VertexModel>> meshes;

	Transform transform;

	std::vector<glm::vec3> positions;

	std::string directory;

	ActorType type;

protected:

	std::vector<std::shared_ptr<Material>> materials;
};


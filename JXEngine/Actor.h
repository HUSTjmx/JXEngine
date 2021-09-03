#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>
#include <vector>

class Material;
class VertexModel;
class Camera;
class Texture;

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



class Actor
{
public:
	Actor(std::shared_ptr<Material> m);

	Actor(const std::string& path, std::shared_ptr<Material> m);

	Actor(std::shared_ptr<VertexModel> m, std::shared_ptr<Material> n);

	Actor(const Actor& a) : meshes(a.meshes), material(a.material), transform(a.transform), positions(a.positions) {}

	Actor& operator=(const Actor& a);

	virtual void AddMesh(std::shared_ptr<VertexModel> m);

	virtual void SetMaterial(std::shared_ptr<Material>);

	virtual void SetPostionsArray(std::vector<glm::vec3>& p);

	virtual std::shared_ptr<Material> GetMaterial();

	virtual void Draw();

	virtual void Delete();

	virtual void Anima(glm::vec3 pos_delta, glm::vec3 sca_delta = glm::vec3(0.0), float angle = 0.0f, glm::vec3 axis = glm::vec3(0.0, 1.0, 0.0));
	
	virtual void AnimaTo(glm::vec3 pos, glm::vec3 scale = glm::vec3(1.0), float angle = 0.0f, glm::vec3 axis = glm::vec3(0.0, 1.0, 0.0));

	virtual void ScaleTo(glm::vec3 scale);

	virtual Transform& TRANSFORM()
	{
		return transform;
	}

	void loadModel(std::string const& path);

	void processNode(aiNode* node, const aiScene* scene);

	std::shared_ptr<VertexModel> processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

private:

	void InitPosition();

	std::vector<std::shared_ptr<VertexModel>> meshes;

	std::shared_ptr<Material> material;

	Transform transform;

	std::vector<glm::vec3> positions;

	std::string directory;
};


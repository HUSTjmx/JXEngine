#pragma once
#include <memory>
#include <vector>

class Camera;
class Actor;
class Light;
class ShaderCompiler;
class Material;
class UniformBuffer;

class Scene
{
public:

	Scene(std::shared_ptr<Camera> camera);

	Scene& operator=(const Scene& scene);
 
	virtual void AddActor(std::shared_ptr<Actor> a);

	virtual void AddLight(std::shared_ptr<Light> l);

	void AddUniformBuffer(std::shared_ptr<UniformBuffer> ub);

	virtual void Draw();

	virtual void Draw(std::shared_ptr<Material> newMat);

	virtual void Delete();

	virtual Camera& MainCamera();

	virtual std::shared_ptr<Actor> GetActorByIndex(int index);

	virtual void LoadLightInfo(std::shared_ptr<Material> shader);

	virtual void LoadCameraInfo(std::shared_ptr<Material> material);

	// Find uniform buffer with the same name.
	// If not find, return -1.
	//...
	int FindUniformBuffer(const std::string& name);

	//...
	std::vector<std::shared_ptr<Actor>>& _Actors_();

	unsigned int ubo;

private:

	std::shared_ptr<Camera> camera;

	std::vector<std::shared_ptr<Actor>> actors;

	std::vector<std::shared_ptr<Light>> lights;

	// Uniform buffer blocks in shader.
	// .
	std::vector<std::shared_ptr<UniformBuffer>> uniformBuffers;

};


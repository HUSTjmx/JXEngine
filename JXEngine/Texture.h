#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

class ShaderCompiler;

class Texture
{
public:
	Texture(): texture(-1), uniformPos(-1), path(""), shaderName("") {}

	Texture(unsigned int tex) : texture(tex), uniformPos(0), path(""), shaderName("texture1") {}

	Texture(const std::string& path, const std::string& directory, bool gamma, GLenum warpMode);

	Texture(int uniformPos, const std::string& s_name, GLenum warpMode);

	Texture(int uniformPos, const std::string& address, const std::string& s_name, GLenum warpMode);

	virtual ~Texture() = default;

	void LoadAndGenerateTex(const std::string& address);

	virtual void Bind() const;

	void SetUniformPos(int newPos);

	void SetForShader(const ShaderCompiler& shader);

	std::string GetPath() const;

	void SetPath(const std::string& path_);

	std::string GetType() const;

	void SetType(const std::string& type_);

	// Copy self, but only transport texture_id and shaderName.
	// Other data do not need.
	// ...
	std::shared_ptr<Texture> Copy() const;

	// ...
	unsigned int Self()const;

	virtual unsigned int loadCubemap(std::vector<std::string> faces) { return -1; }

protected:

	unsigned int texture;

	unsigned int uniformPos;

	std::string shaderName;

	std::string path;
};


class CubeTexture : public Texture
{
public:

	CubeTexture() : Texture() {}

	CubeTexture(const std::string& name) : Texture() {
		SetUniformPos(0);
		SetType(name);
	}

	unsigned int loadCubemap(std::vector<std::string> faces) override;

	virtual void Bind()const override;
};

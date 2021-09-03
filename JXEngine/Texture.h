#pragma once
#include <glad/glad.h>
#include <string>

class ShaderCompiler;

class Texture
{
public:
	Texture(): texture(-1), uniformPos(-1), path(""), shaderName("") {}

	Texture(unsigned int tex) : texture(tex), uniformPos(0), path(""), shaderName("texture1") {}

	Texture(const std::string& path, const std::string& directory, bool gamma, GLenum warpMode);

	Texture(int uniformPos, const std::string& s_name, GLenum warpMode);

	Texture(int uniformPos, const std::string& address, const std::string& s_name, GLenum warpMode);

	void LoadAndGenerateTex(const std::string& address);

	void Bind() const;

	void SetUniformPos(int newPos);

	void SetForShader(const ShaderCompiler& shader);

	std::string GetPath() const;

	void SetPath(const std::string& path_);

	std::string GetType() const;

	void SetType(const std::string& type_);


private:

	unsigned int texture;

	unsigned int uniformPos;

	std::string shaderName;

	std::string path;
};


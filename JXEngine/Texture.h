// Copyright 2021.	All rights reserved
//
// Author: 2568094892@qq.com (Jiang Meng Xian)
//
// this file include texture base class and derivative class, e.g. cubeMap etc.
// ...

#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

class ShaderCompiler;

// Texture is the object that represent the picture used in the shader, or as the assignment of the buffer.
// The texture class is handle under the material or the frameBuffer, 
// and a texture object should be used for many material.
// ...
class Texture
{
public:
	Texture(): texture(-1), uniformPos(-1), path(""), shaderName("") {}

	Texture(unsigned int tex) : texture(tex), uniformPos(0), path(""), shaderName("texture1") {}

	Texture(const std::string& path, const std::string& directory, bool gamma, GLenum warpMode);

	Texture(int uniformPos, const std::string& s_name, GLenum warpMode);

	Texture(int uniformPos, const std::string& address, const std::string& s_name, GLenum warpMode);

	virtual ~Texture() = default;

	// Load the picture to the texture object.
	// Construct function will auto draw it.
	// ...
	void LoadAndGenerateTex(const std::string& address);

	// Bind the texture to texture pool pos in the rendering.
	// ...
	virtual void Bind() const;

	void SetUniformPos(int newPos);

	// Set the texture for shader object.
	// The function should be used after adding the texture to material that is not on the fly. 
	// ...
	void SetForShader(const ShaderCompiler& shader);

	// ...
	std::string GetPath() const;

	// ...
	void SetPath(const std::string& path_);

	// Get the shaderName.
	// ...
	std::string GetType() const;

	// Set the shaderName.
	// In more detail, set the texture name in the shader.
	// ...
	void SetType(const std::string& type_);

	// Copy self, but only transport texture_id and shaderName.
	// Other data do not need.
	// ...
	std::shared_ptr<Texture> Copy() const;

	// Return handle of the texture.
	// ...
	unsigned int Self()const;

	// Load cube map texture.
	// This will be reloaded by the CubeTexture Class.
	// ...
	virtual unsigned int loadCubemap(std::vector<std::string> faces) { return -1; }

protected:

	// The opengl Handle.
	// ...
	unsigned int texture;

	// The texture pos in the texture pool.
	// ...
	unsigned int uniformPos;

	// The texture name in the shader.
	// e.g. uniform sampler2D albedoTexture. 
	// ...
	std::string shaderName;

	// The file path of the picture
	// ...
	std::string path;
};


// CubeTexture is the derivative class of the texture.
// It is usually used for Sky box.
// ...
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

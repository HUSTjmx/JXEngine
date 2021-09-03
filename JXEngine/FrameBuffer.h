#pragma once
#include <glad/glad.h>
#include <memory>
#include <vector>

class Texture;

class FrameBuffer
{
public:

	FrameBuffer();

	FrameBuffer(unsigned int width, unsigned int height);

	void AddTexture(GLenum format);

	void AddTexture(GLenum format, std::string type_);

	void NotifyGL();

	void AddRenderObject(bool hasStencil);

	void Delete();

	std::vector<std::shared_ptr<Texture>> textureBuffers;

private:

	unsigned int BindTextureToBuffer(GLenum format);

	unsigned int fbo;

	unsigned int rbo;

	unsigned int height, width;

	bool hasStencil;
	
	std::vector<GLuint> attachments;
};


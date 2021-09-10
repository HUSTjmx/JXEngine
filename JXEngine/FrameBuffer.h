#pragma once
#include <glad/glad.h>
#include <memory>
#include <vector>
#include <string>

class Texture;

class FrameBuffer
{
public:

	FrameBuffer();

	FrameBuffer(unsigned int width, unsigned int height);

	void AddTexture(GLenum format, bool isMSAA = false);

	void AddTexture(GLenum format, std::string type_, bool isMSAA = false);

	void NotifyGL();

	void AddRenderObject(bool hasStencil, bool isMSAA = false);

	void Delete();

	unsigned int FBO()const;

	std::vector<std::shared_ptr<Texture>> textureBuffers;

private:

	unsigned int BindTextureToBuffer(GLenum format, bool isMSAA = false);

	unsigned int fbo;

	unsigned int rbo;

	unsigned int height, width;

	bool hasStencil;
	
	std::vector<GLuint> attachments;
};




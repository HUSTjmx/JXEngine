// Copyright 2021.	All rights reserved
//
// Author: 2568094892@qq.com (Jiang Meng Xian)
//
// this file include texture base class and derivative class, e.g. cubeMap etc.
// ...
#pragma once
#include <glad/glad.h>
#include <memory>
#include <vector>
#include <string>

class Texture;

// FrameBuffer is the class that Represents a frame buffer, currently designed primarily as a member of the Pass class, 
// store the results of a Pass rendering.
// We should call AddTexture*, NotifyGL, AddRenderObject(Optional) after we create a instance if we want to use it correctly.
// ...
class FrameBuffer
{
public:

	FrameBuffer();

	FrameBuffer(unsigned int width, unsigned int height);

	// Add a new color attachments.
	// ...
	void AddTexture(GLenum format, bool isMSAA = false);

	// Add a new color attachments.
	// ...
	void AddTexture(GLenum format, std::string type_, bool isMSAA = false);

	// Register these frame buffers to opengl.
	// ...
	void NotifyGL();

	// Bind a depth and template rendering buffer object.
	// ...
	void AddRenderObject(bool hasStencil, bool isMSAA = false);

	// Transport the data of one of the attachments in the FrameBuffer to the Texture Obj.
	// ...
	void CopyColorAttachmentToTex(GLuint dest, unsigned int index);

	// ...
	void Delete();

	// Return handle of Framebuffer.
	// ...
	unsigned int FBO()const;

	// The textureBuffer of attachments.
	// ...
	std::vector<std::shared_ptr<Texture>> textureBuffers;

private:

	// ...
	unsigned int BindTextureToBuffer(GLenum format, bool isMSAA = false);

	// The handle of FrameBuffer.
	// ...
	unsigned int fbo;

	// ...
	unsigned int rbo;

	// ...
	unsigned int height, width;

	// Is have stencil buffer in the FrameBuffer.
	// ...
	bool hasStencil;
	
	// ...
	std::vector<GLuint> attachments;
};




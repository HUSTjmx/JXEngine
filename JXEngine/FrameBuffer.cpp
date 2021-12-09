#include <iostream>
#include "FrameBuffer.h"
#include "Config.h"
#include "Texture.h"

GLuint GetAttachmentByIndex(int index)
{
	switch (index)
	{
	case 0:
		return GL_COLOR_ATTACHMENT0;
		break;
	case 1:
		return GL_COLOR_ATTACHMENT1;
		break;
	case 2:
		return GL_COLOR_ATTACHMENT2;
		break;
	case 3:
		return GL_COLOR_ATTACHMENT3;
		break;
	case 4: 
		return GL_COLOR_ATTACHMENT4;
		break;
	case 5:
		return GL_COLOR_ATTACHMENT5;
	case 6:
		return GL_COLOR_ATTACHMENT6;
	case 7:
		return GL_COLOR_ATTACHMENT7;
	default:
		break;
	}
}

std::string GenTextureName(int index)
{
	return "texture" + index;
}

FrameBuffer::FrameBuffer()
{
	//创建一个帧缓冲对象
	glGenFramebuffers(1, &fbo);

	//绑定帧缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	width = CONFIG::SCREEN_CONFIG::SCR_WIDTH;

	height = CONFIG::SCREEN_CONFIG::SCR_HEIGHT;

	hasStencil = false;
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
{
	// 创建一个帧缓冲对象
	glGenFramebuffers(1, &fbo);

	//绑定帧缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	this->width = width;

	this->height = height;

	hasStencil = true;
}

void FrameBuffer::AddTexture(GLenum format, bool isMSAA)
{
	//绑定帧缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	auto texture = BindTextureToBuffer(format, isMSAA);
	
	auto textureBuffer = std::make_shared<Texture>(texture);

	textureBuffer->SetType(GenTextureName(textureBuffers.size()));

	textureBuffers.push_back(textureBuffer);
}

void FrameBuffer::AddTexture(GLenum format, std::string type_, bool isMSAA)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	auto texture = BindTextureToBuffer(format, isMSAA);

	auto textureBuffer = std::make_shared<Texture>(texture);

	textureBuffer->SetType(type_);

	textureBuffers.push_back(textureBuffer);
}

void FrameBuffer::NotifyGL()
{
	glDrawBuffers(attachments.size(), &attachments[0]);
}

void FrameBuffer::AddRenderObject(bool hasStencil, bool isMSAA)
{
	this->hasStencil = hasStencil;

	glGenRenderbuffers(1, &rbo);
	
	//绑定这个渲染缓冲对象
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	
	if (hasStencil)
	{
		if (!isMSAA)
		{
			//std::cout << isMSAA << std::endl;
			//创建一个深度和模板渲染缓冲对象
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
		}
		else
		{
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
		}
		//附加这个渲染缓冲对象
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	}
	else
	{
		if (!isMSAA)
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		else
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void FrameBuffer::CopyColorAttachmentToTex(GLuint dest, unsigned int index)
{
	/*glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	auto a = GetAttachmentByIndex(index);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, a, GL_TEXTURE_2D, dest, 0);
	glBindTexture(GL_TEXTURE_2D, dest);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);*/

	glBindBuffer(GL_COPY_READ_BUFFER, textureBuffers[index]->Self());
	glBindBuffer(GL_COPY_WRITE_BUFFER, dest);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, width * height);
}

void FrameBuffer::Delete()
{
	glDeleteFramebuffers(1, &fbo);
}

unsigned int FrameBuffer::FBO() const
{
	return fbo;
}

unsigned int FrameBuffer::BindTextureToBuffer(GLenum format, bool isMSAA)
{
	//std::cout << isMSAA << std::endl;
	auto type_ = GL_TEXTURE_2D;

	if (isMSAA)
		type_ = GL_TEXTURE_2D_MULTISAMPLE;

	//为帧缓冲创建一个纹理
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(type_, texture);

	if (isMSAA)
		glTexImage2DMultisample(type_, 4, format, width, height, GL_TRUE);
	else
		glTexImage2D(type_, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(type_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(type_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(type_, GL_TEXTURE_BORDER_COLOR, borderColor);

	auto attachment = GetAttachmentByIndex(attachments.size());
	if (format == GL_DEPTH_COMPONENT) attachment = GL_DEPTH_ATTACHMENT;
	attachments.push_back(attachment);
	//附加到帧缓冲上
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, type_, texture, 0);

	return texture;
}

//std::shared_ptr<Texture> FrameBuffer::GetColorTextureBuffer()
//{
//	return textureBuffer;
//}

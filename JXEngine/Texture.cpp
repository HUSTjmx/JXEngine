#include <iostream>
#include <string>
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ShaderCompiler.h"
#include "Config.h"

Texture::Texture(const std::string& shader_name)
{
	this->uniformPos = 0;
	shaderName = shader_name;
	path = "";

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

Texture::Texture(const std::string& path, const std::string& directory, bool gamma, GLenum warpMode)
{
	uniformPos = 0;

	std::string filename = path;
	filename = directory + '/' + filename;

	glGenTextures(1, &texture);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warpMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warpMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

}

Texture::Texture(int uniformPos, const std::string& s_name, GLenum warpMode)
{
	this->uniformPos = uniformPos;
	shaderName = s_name;
	path = "";

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warpMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warpMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(int uniformPos, const std::string & address, const std::string& s_name, GLenum warpMode)
{
	this->uniformPos = uniformPos;
	shaderName = s_name;
	path = address;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warpMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warpMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	LoadAndGenerateTex(address);
}

void Texture::LoadAndGenerateTex(const std::string& address)
{
	// 加载并生成纹理
	int width, height, nrChannels;
	unsigned char* data = stbi_load(address.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void Texture::SetUniformPos(int newPos)
{
	uniformPos = newPos;
}

void Texture::SetForShader(const ShaderCompiler& shader)
{
	shader.UseSelf();
	shader.SetInt(shaderName, uniformPos);
}

std::string Texture::GetPath() const
{
	return path;
}

void Texture::SetPath(const std::string& path_)
{
	path = path_;
}

std::string Texture::GetType() const
{
	return shaderName;
}

void Texture::SetType(const std::string& type_)
{
	//std::cout << type_ << std::endl;
	shaderName = type_;
}

std::shared_ptr<Texture> Texture::Copy() const
{
	std::shared_ptr<Texture> t = std::make_shared<Texture>();
	t->texture = this->texture;
	t->shaderName = shaderName;
	return t;
}

unsigned int Texture::Self() const
{
	return texture;
}

void Texture::Bind() const
{
	switch (uniformPos)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	case 1:
		glActiveTexture(GL_TEXTURE1);
		break;
	case 2:
		glActiveTexture(GL_TEXTURE2);
		break;
	case 3:
		glActiveTexture(GL_TEXTURE3);
		break;
	case 4:
		glActiveTexture(GL_TEXTURE4);
		break;
	case 5:
		glActiveTexture(GL_TEXTURE5);
		break;
	case 6:
		glActiveTexture(GL_TEXTURE6);
		break;
	case 7:
		glActiveTexture(GL_TEXTURE7);
		break;
	default:
		break;
	}
	glBindTexture(GL_TEXTURE_2D, texture);
}

unsigned int CubeTexture::loadCubemap(std::vector<std::string> faces)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}

void CubeTexture::Bind() const
{
	switch (uniformPos)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	case 1:
		glActiveTexture(GL_TEXTURE1);
		break;
	case 2:
		glActiveTexture(GL_TEXTURE2);
		break;
	case 3:
		glActiveTexture(GL_TEXTURE3);
		break;
	case 4:
		glActiveTexture(GL_TEXTURE4);
		break;
	case 5:
		glActiveTexture(GL_TEXTURE5);
	default:
		break;
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}

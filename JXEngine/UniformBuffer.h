// Copyright 2021.	All rights reserved
//
// Author: 2568094892@qq.com (Jiang Meng Xian)
// ...

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

// UnidormData Type.
// Not including Array Type
// ...
enum class UniformDataType
{
	Float,
	Vector3,
	Mat4,
	Bool,
	Int
};


// UniformData is the data that we need place at UniformBuffer by the layout.
// 
// But in true, we1 do not load any data in it, we just use it to compute offset,
// and make sure to bind data to right pos in shader.
// ...
struct UniformData
{
	UniformData(std::string n, UniformDataType m) : name(n), type(m), size(0), offset(0) {}

	// Corresponding Name in shader.
	// ...
	std::string name;

	// The size of this data.
	// ...
	unsigned int size;

	// The offset in the UniformBuffer.
	// ...
	unsigned int offset;

	UniformDataType type;
};


// Uniform Buffer Object, allow us define some global uniform variables shared with many shaders, 
// e.g. view, projection, viewPos etc.. At most case, 
// we only need init it once at the head of the render.
//
// Because it is global, so we should place it at scene level, 
// usually as the element of the scene object.
//
// We should init and set this class before doing so for ShaderCompiler class, 
// because we need transform some info of it to this shader for binding block.
// 
// Before we create it, we should firstly add all data belong to it, 
// because we need compute the size of uniform buffer.
// ...
#pragma once
class UniformBuffer
{
public:

	UniformBuffer() : ubo(-1), size(0), name("") {}

	UniformBuffer(std::string name_) : ubo(-1), size(0), name(name_) {}

	// Create the uniform buffer, init the ubo.
	// ...
	void Create();

	// Define the range of the buffer that links to a uniform binding point.
	//
	// Input:
	//   <pos>: xth binding point, Default = 0.
	//   <offset>: the offset away from the start of the binding point, Default = 0.
	// ...
	void LinkBindingPoint(unsigned int pos = 0 , unsigned int offset = 0);

	// Add a item to data, update the size.
	// ...
	void AddData(UniformData data);

	// Store the real data to buffer according its name.
	// We need select different handle, e.g. vec3, mat4 etc..
	//
	// Input:
	//   <v>: the real data for storing.
	//   <name_>: string, id for finding the right pos to store.
	// ...
	template<typename T>
	void StoreData(T v, const std::string& name_);

	std::string GetName()const;

private:

	// the handle of this Uniform buffer, we use it as the buffer.
	// ...
	unsigned int ubo;

	// The size of uniform buffer.
	// ...
	unsigned int size;

	// The name of UniformBuffer, should equal this name of uniform block in shader file
	// ...
	std::string name;

	// Datas in the UniformBuffer
	// ...
	std::vector<UniformData> datas;
	
};

template<typename T>
inline void UniformBuffer::StoreData(T v, const std::string& name_)
{
	for (auto i = datas.begin(); i != datas.end(); ++i)
	{
		if (std::strcmp(name_.c_str(), i->name.c_str()) == 0)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, i->offset, i->size, (void*)v);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}
}

template<>
inline void UniformBuffer::StoreData<glm::mat4>(glm::mat4 v, const std::string& name_)
{
	for (auto i = datas.begin(); i != datas.end(); ++i)
	{
		if (std::strcmp(name_.c_str(), i->name.c_str()) == 0)
		{
			//std::cout << ubo <<"  "<<i->offset<< std::endl;
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, i->offset, i->size, glm::value_ptr(v));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}
}

template<>
inline void UniformBuffer::StoreData<glm::vec3>(glm::vec3 v, const std::string& name_)
{
	for (auto i = datas.begin(); i != datas.end(); ++i)
	{
		if (std::strcmp(name_.c_str(), i->name.c_str()) == 0)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, i->offset, i->size, glm::value_ptr(v));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}
}


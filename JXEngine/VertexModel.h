// Copyright 2021.	All rights reserved
//
// Author: 2568094892@qq.com (Jiang Meng Xian)
// ...
#pragma once
#include "std.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Default properties of model vertices, now including pos,
// normal, texcoords, tangent, bitangent.
// ...
struct Vertex {

	// position
	// ...
	glm::vec3 Position;

	// normal
	// ...
	glm::vec3 Normal;

	// texCoords
	// ...
	glm::vec2 TexCoords;

	// tangent
	// ...
	glm::vec3 Tangent;

	// bitangent
	// ...
	glm::vec3 Bitangent;
};


// The actor there has two part, vertexModel that means mesh, 
// and the material that decide how to draw mesh.
//
// vertexModel is the most base draw in the list of draw, and has two mode,
// draw normally, and draw by indices.
// ...
class VertexModel
{
public:

	VertexModel() : VBO(0), VAO(0), EBO(0), haveIndices(false), IsInstanced(false), attriPointerPos(0) {}

	// Bind vertex data to the buffer object(VBO), when we bind a VAO.
	// We should firstly call this or it`s variants after we create a instance.
	// 
	// We will set haveIndices <false>.
	// 
	// Input:
	//   <vertices>: float[N], the vertices data we need.
	//   <offsets>: uint[F], the layout of data.
	//
	// Heavy load 1 : no indices.
	// ...
	template<size_t N, size_t F>
	void BindVertexToBuffer(float(&vertices)[N], unsigned int(&offsets)[F]);

	// Bind vertex data to the buffer object(VBO) and use EBO, when we bind a VAO.
	// We should firstly call this or it`s variants after we create a instance.
	// 
	// We will set haveIndices <True>.
	// 
	// Input:
	//   <vertices>: float[N], the vertices data we need.
	//   <indices>: uint[M], the indices data we need.
	//   <offsets>: uint[F], the layout of data.
	//
	// Heavy load 2 : have indices.
	// ...
	template<size_t N, size_t M, size_t F>
	void BindVertexToBuffer(float(&vertices)[N], unsigned int(&indices)[M], unsigned int(&offsets)[F]);

	// Bind vertex data to the buffer object(VBO) and use EBO, when we bind a VAO.
	// We should firstly call this or it`s variants after we create a instance.
	// 
	// We will set haveIndices <True>.
	// 
	// Input:
	//   <vertices>: vector<Vertex>, the vertices data we need.
	//   <indices>: vector<uint>, the indices data we need.
	//
	// Heavy load 3 : have indices.
	// ...
	void BindVertexToBuffer(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	// Bind vertex data to the buffer object(VBO) and use EBO, when we bind a VAO.
	// We should firstly call this or it`s variants after we create a instance.
	// 
	// We will set haveIndices <True>.
	// 
	// Input:
	//   <vertices>: vector<float>, the vertices data we need.
	//   <offsets>: vector<uint>, the layout of data.
	//
	// Heavy load 4 : no indices.
	// ...
	void BindVertexToBuffer(std::vector<float> vertices, std::vector<unsigned int> offsets);

	// Bind vertex data to the buffer object(VBO) and use EBO, when we bind a VAO.
	// We should firstly call this or it`s variants after we create a instance.
	// 
	// We will set haveIndices <True>.
	// 
	// Input:
	//   <vertices>: vector<float>, the vertices data we need.
	//   <indices>: vector<uint>, the indices data we need.
	//   <offsets>: vector<uint>, the layout of data.
	//
	// Heavy load 5 : have indices.
	// ...
	void BindVertexToBuffer(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<unsigned int> offsets);

	// Set some data (buffer) as an instance vertex attribute (with divisor 1).
	//
	// Input:
	//   <buffer>: uint, the buffer binding with the data for vertex attribute.
	//   <offsets>: the layout of data in the buffer, e.g. [vec3, vec4] --> [3, 4].
	//
	// note: the base unit of data is Float. 
	// ...
	template<size_t F>
	void AddAttribute(unsigned int buffer, unsigned int(&offsets)[F]);

	// Bind this VAO, so we can draw it. 
	// ... 
	void BindVAO();

	// Draw self, and according the data we have, we will select different draw mode.
	// 1: DrawBase.
	// 2: DrawIndex.
	// ...
	void Draw();

	// Delete the buffer we have get.
	// ...
	void Delete();

	//...
	void SetInstanceNum(unsigned int num);

	unsigned int GetVAO();

private:

	// Draw with indices data.
	// ...
	void DrawIndex();

	// Draw with only vertices data.
	// ...
	void DrawBase();

	// the buffer handle.
	// ...
	unsigned int VBO, VAO, EBO;

	// is draw by indices?
	// <true> means drawing by indices.
	// ...
	bool haveIndices;

	// Is Draw instanced?
	// ...
	bool IsInstanced;

	// The number of instanced.
	//...
	unsigned int instanceSize;

	// The number of vertexArray.
	// ...
	unsigned int vertexSize;
	
	// The number of indicesArray.
	// ...
	unsigned int indicesSize;

	// Current attribute pointers pos.
	// ...
	unsigned int attriPointerPos;
};


template<size_t N, size_t F>
inline void VertexModel::BindVertexToBuffer(float(&vertices)[N], unsigned int(&offsets)[F])
{
	//生成一个VBO对象
	glGenBuffers(1, &VBO);

	//生成一个VAO对象
	glGenVertexArrays(1, &VAO);

	//绑定顶点数组对象
	glBindVertexArray(VAO);

	//把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//把用户定义的数据复制到当前绑定缓冲
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	int sum = 0;
	for (int i = 0;i < F;i++)sum += offsets[i];
	int k = 0;
	for (int i = 0;i < F;i++) 
	{
		//设置顶点属性指针
		glVertexAttribPointer(i, offsets[i], GL_FLOAT, GL_FALSE, sum * sizeof(float), (void*)(k * sizeof(float)));
		glEnableVertexAttribArray(i);
		k += offsets[i];
	}

	//----------------------------------------------------------------------------

	haveIndices = false;
	indicesSize = 0;
	vertexSize = N / sum;
	attriPointerPos = F;
	//std::cout << F << std::endl;
}

template<size_t N, size_t M, size_t F>
inline void VertexModel::BindVertexToBuffer(float(&vertices)[N], unsigned int(&indices)[M], unsigned int(&offsets)[F])
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	int sum = 0;
	for (int i = 0;i < F;i++)sum += offsets[i];
	int k = 0;
	for (int i = 0;i < F;i++)
	{
		//设置顶点属性指针
		glVertexAttribPointer(i, offsets[i], GL_FLOAT, GL_FALSE, sum * sizeof(float), (void*)(k * sizeof(float)));
		glEnableVertexAttribArray(i);
		k += offsets[i];
	}

	vertexSize = N / sum;
	indicesSize = M;
	attriPointerPos = F;
	haveIndices = true;

	
}

template<size_t F>
inline void VertexModel::AddAttribute(unsigned int buffer, unsigned int(&offsets)[F])
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	int sum = 0;
	for (int i = 0;i < F;i++)sum += offsets[i];
	int k = 0;

	for (int i = 0;i < F;i++)
	{
		//设置顶点属性指针
		glVertexAttribPointer(i + attriPointerPos, offsets[i], GL_FLOAT, GL_FALSE, sum * sizeof(float), (void*)(k * sizeof(float)));
		glEnableVertexAttribArray(i + attriPointerPos);
		k += offsets[i];
	}

	for (int i = 0;i < F;i++)
		glVertexAttribDivisor(i + attriPointerPos, 1);

	attriPointerPos += F;

	glBindVertexArray(0);
}


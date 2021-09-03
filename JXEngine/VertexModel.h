#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};

class VertexModel
{
public:

	VertexModel() : VBO(0), VAO(0), EBO(0), haveIndices(false) {}

	template<size_t N, size_t F>
	void BindVertexToBuffer(float(&vertices)[N], unsigned int(&offsets)[F]);

	template<size_t N, size_t M, size_t F>
	void BindVertexToBuffer(float(&vertices)[N], unsigned int(&indices)[M], unsigned int(&offsets)[F]);

	void BindVertexToBuffer(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	void BindVAO();

	void Draw();

	void Delete();

private:

	void DrawIndex();

	void DrawBase();

	unsigned int VBO, VAO, EBO;

	//是否是按索引的方式进行
	bool haveIndices;

	unsigned int vertexSize, indicesSize;
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
	haveIndices = true;
}

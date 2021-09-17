#include "VertexModel.h"
#include <type_traits>


void VertexModel::BindVertexToBuffer(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	vertexSize = vertices.size();
	indicesSize = indices.size();
	haveIndices = true;
	attriPointerPos = 5;

	// create buffers / arrays
    glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about struct is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);

	//std::cout << vertexSize << std::endl;
}

void VertexModel::BindVertexToBuffer(std::vector<float> vertices, std::vector<unsigned int> offsets)
{
	int N = vertices.size();
	int F = offsets.size();

	//std::cout << N << " " << F << std::endl;

	//生成一个VBO对象
	glGenBuffers(1, &VBO);

	//生成一个VAO对象
	glGenVertexArrays(1, &VAO);

	//绑定顶点数组对象
	glBindVertexArray(VAO);

	//把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//把用户定义的数据复制到当前绑定缓冲
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

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
}

void VertexModel::BindVertexToBuffer(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<unsigned int> offsets)
{
	int N = vertices.size();
	int M = indices.size();
	int F = offsets.size();

	//std::cout << vertices.size() * sizeof(float) << " " << M << std::endl;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

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

void VertexModel::BindVAO()
{
	glBindVertexArray(VAO);
}

void VertexModel::Draw()
{
	if (!haveIndices)
		DrawBase();
	else
		DrawIndex();
}

void VertexModel::DrawIndex()
{
	//std::cout << instanceSize << std::endl;
	if (!IsInstanced)
		glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
	else
		glDrawElementsInstanced(
			GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0, instanceSize
		);
}

void VertexModel::DrawBase()
{
	if(!IsInstanced)
		glDrawArrays(GL_TRIANGLES, 0, vertexSize);
	else
		glDrawArraysInstanced(GL_TRIANGLES, 0, vertexSize, instanceSize);
}

void VertexModel::Delete()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void VertexModel::SetInstanceNum(unsigned int num)
{
	instanceSize = num;

	if (instanceSize > 1) IsInstanced = true;
	else IsInstanced = false;
}

unsigned int VertexModel::GetVAO()
{
	return VAO;
}


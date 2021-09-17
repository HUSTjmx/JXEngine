// Copyright 2021.	All rights reserved
//
// Author: 2568094892@qq.com (Jiang Meng Xian)
//
// this files include some tool class, for some frequently used functions, for
// example : StringTool, MatTool, BufferTool ect.
// ...

#pragma once
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


//namespace StructureTool {
//	template<class T, size_t N >
//	constexpr size_t GetArrayCount(T(&arr)[N]) noexcept
//	{
//		return N;
//	}
//}


// Some string operations that are not included in stl.
// Use Singleton Pattern, so can use anywhere, 
// and you can not create any object of this class outside it
// 
// Example:
//    StringTool::Instance().DelLineFromFile(file, 3);
//	  StringTool st; this is error!
// ...
class StringTool
{

public:

	// Return this only instance of the class.
	StringTool& Instance()
	{
		static StringTool* instance = new StringTool();
		return *instance;
	}

	// Return a string after deleting one line that you do not need.
	//
	// Input:
	//   <file> : std::ifstream&, a opened file stream.
	//   <num> : unsigned int, line num of delete.
	//
	// Warn: the length of a line must less 1024.
	// ...
	std::string DelLineFromFile(std::ifstream& file, unsigned int num);

private:

	StringTool();
};


// Sort the array of pos(vec3) by the distance from specific location.
//
// Input:
//   <v>: vector<vec3>&,  e.g.the position of the transparent triangular piece element
//   <c_pos>: vec3,  e.g. the camera pos
//
// This function will change this <v>, by small to big.
// ...
extern void SortPosByDistance(std::vector<glm::vec3>& v, glm::vec3 c_pos);


// Some mat operations that are not included in glm.
// Use Singleton Pattern, so can use anywhere, 
// and you can not create any object of this class outside it
// 
// Example:
//    MatTool::Instance().RemoveTranslation(model);
// 	  MatTool mt; this is error!
// ...
class MatTool
{
public:

	// Return this only instance of the class.
	static MatTool& Instance()
	{
		static MatTool* intance_ = new MatTool();
		return *intance_;
	}

	// Remove translation part of this mat4.
	// 
	// Input:
	//   <a>: mat4, e.g. this view matrix when we render sky box.
	// ...
	glm::mat4 RemoveTranslation(glm::mat4 a);

	//Generate a large list of semi-random model transformation matrices.
	//
	// Input:
	//   <radius>: float, the range of pos.
	//   <offset>: float, .
	//
	// template: 
	//   <amount>: the number of mat.
	//...
	template<size_t amount>
	std::shared_ptr<glm::mat4> GenerateModelMat_R(float radius, float offset);

private:

	MatTool() {}
};

// Some OpenGL buffer operations that are not included in Frame class etc..
// Use Singleton Pattern, so can use anywhere, 
// and you can not create any object of this class outside it
// 
// Example:
//    BufferTool::Instance().xxxx();
//	  BufferTool mt; this is error!
// ...
class BufferTool 
{
public:

	enum
	{
		N = 4  // Most basic unit of std140 base Alignment
	};

	// Return this only instance of the class.
	static BufferTool& Instance()
	{
		static BufferTool* instance_ = new BufferTool();
		return *instance_;
	}
	
	// Copy array data (e.g. from file) to buffer.
	//
	// Input:
	//   <buffer>: unsigned int&, the buffer that is filled.
	//   <data>: T[N], the data that we need fill.
	// 
	// Warn: the typename T is usually float.
	// ...
	template<typename T, size_t M>
	void CopyDataToArrayBuffer(unsigned int& buffer, T(*data)[M]);

	// Create a array buffer for this data.
	// 
	// Input:
	//   <a>: mat4*, the array data for binding
	//   <size>: int, size of the array data.
	// ...
	template<typename T>
	unsigned int CreateArrayBuffer(std::shared_ptr<T> a, int size);

	// According to the type of T, return the size of Block,
	// on the other word, return the base alignment.
	// 
	// There are many template specialisation.
	// 
	// The rules can see in https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/08%20Advanced%20GLSL/
	// ...
	template<typename T,size_t NUM = 1>
	unsigned int GetBaseAlignment_std140();
	
private:
	BufferTool() {}
};

template<typename T, size_t M>
inline void BufferTool::CopyDataToArrayBuffer(unsigned int& buffer, T(*data)[M])
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	
	//get buffet ptr
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	//copy data to buffer
	memcpy(ptr, data, N);

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

template<typename T>
inline unsigned int BufferTool::CreateArrayBuffer(std::shared_ptr<T> a, int size)
{
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(T), &a.get()[0], GL_STATIC_DRAW);
	return buffer;
}


template<typename T, size_t NUM>
inline unsigned int BufferTool::GetBaseAlignment_std140()
{
	return NUM * N * 4;
}

template<>
inline unsigned int BufferTool::GetBaseAlignment_std140<int, 1>()
{
	return 4;
}

template<>
inline unsigned int BufferTool::GetBaseAlignment_std140<bool, 1>()
{
	return 4;
}

template<>
inline unsigned int BufferTool::GetBaseAlignment_std140<float, 1>()
{
	return 4;
}

template<size_t amount>
inline std::shared_ptr<glm::mat4> MatTool::GenerateModelMat_R(float radius, float offset)
{
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed	
	
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	return std::shared_ptr<glm::mat4>(modelMatrices);
}




// Sphere Creator.
// ...
class SphereCreator 
{
public:

	static SphereCreator& Instance()
	{
		static SphereCreator* instance_ = new SphereCreator();
		return *instance_;
	}

	// Vertex class.
	// There are some data we need, e.g. pos, uv, normal.
	// ...
	struct Vertex {
		glm::vec3 pos;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	// Face class.
	// There are three id, it is enough to show face.
	// ...
	struct face
	{
		int v1;
		int v2;
		int v3;

		face(int i, int j, int k)
		{
			v1 = i;
			v2 = j;
			v3 = k;
		}
	};

	// Create op with ico algorithm.
	// ...
	void Create_ICO(std::vector<float>& v_d, std::vector<unsigned int>& indexs, std::vector<unsigned int>& offsets, int recursionLevel);

private:

	SphereCreator() {}

	// Add vertex to mesh, fix position to be unit sphere.
	// ...
	int AddVertex(glm::vec3 pos);

	// Return index of point in the middle of p1 and p2.
	// ...
	int GetMiddlePoint(int p1, int p2);

	// Vertexs in Sphere.
	// ...
	std::vector<Vertex> vertexs;

	// Current vertex id.
	// ...
	int index;

	// record.
	// ...
	std::map<_int64, int> m_cache;
};
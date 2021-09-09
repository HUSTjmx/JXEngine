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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>


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
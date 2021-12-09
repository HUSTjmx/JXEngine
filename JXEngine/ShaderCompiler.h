// Copyright 2021.	All rights reserved
//
// Author: 2568094892@qq.com (Jiang Meng Xian)
//
// this files include shdaerCompiler class.
// example : StringTool, MatTool, BufferTool ect.
// ...
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

// This class can compile shader file and is used as shader self. The class is one of base class.
// We can set values by it for shader
// The class is always a member of Material Class, and we do not use it only.
// We should call Compile() to compile code.
// ...
class ShaderCompiler
{
public:
	ShaderCompiler(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	ShaderCompiler(const char* vertexCode, const char* fragmentCode, int xx);

	ShaderCompiler(const char* vertexCode, const char* fragmentCode, const char* geometryCode, int xx);

	ShaderCompiler(const ShaderCompiler& other);

	// All instance need call this after setting code.
	// We must call this to ensure code is useful.
	// ...
	void Compile();

	// Let current pass draw objects by this shader.
	// ...
	void UseSelf()const;

	void Delete()const;

	// After we transport main code to this, we need ensure some relied function library is included.
	// Note: the sort of library is inverse, for example: light rely on math, we should firstly add light, then add math.
	// ...
	void AddIncludeFile(const std::string& filePath);

	// ...
	void SetBool(const std::string& name, bool value) const;

	// ...
	void SetInt(const std::string& name, int value) const;

	// ...
	void SetFloat(const std::string& name, float value) const;

	// ...
	void SetVec2(const std::string& name, const glm::vec2& value) const;

	// ...
	void SetVec2(const std::string& name, float x, float y) const;

	// ...
	void SetVec3(const std::string& name, const glm::vec3& value) const;

	// ...
	void SetVec3(const std::string& name, float x, float y, float z) const;

	// ...
	void SetVec4(const std::string& name, const glm::vec4& value) const;

	// ...
	void SetVec4(const std::string& name, float x, float y, float z, float w) const;

	// ...
	void SetMat2(const std::string& name, const glm::mat2& mat) const;

	// ...
	void SetMat3(const std::string& name, const glm::mat3& mat) const;

	// ...
	void SetMat4(const std::string& name, const glm::mat4& mat) const;

	// ...
	template<typename T>
	void SetUniform(const std::string& name, T value, void(*pf)(const std::string&, T));

	// Binding uniform block.
	// ...
	void BlockBindingUniform(unsigned int pos, const char* uniform) const;

	// Return copy of this shader.
	// But we only share the code, do not share status!
	// So we need re-compile() to get new handle of program.
	// ...
	ShaderCompiler Copy();

	// ...
	static std::string FILE_HEAD;

	// The id of shaderProgram.
	// ...
	unsigned int ID;

private:

	// Check errors when compile.
	// ...
	void CheckCompileErrors(GLuint shader, std::string type)const;

	// Is we have geometryShader?
	// ...
	bool hasGeometry;

	// Vertex shader code.
	// ...
	std::string vertexShader;

	// fragment shader code.
	// ...
	std::string fragmentShader;

	// Geometry shader code.
	// ...
	std::string geometryShader;
};

template<typename T>
inline void ShaderCompiler::SetUniform(const std::string& name, T value, void(*pf)(const std::string&, T))
{
	pf(name, value);
}

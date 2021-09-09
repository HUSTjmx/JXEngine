#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>


class ShaderCompiler
{
public:
	ShaderCompiler(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	ShaderCompiler(const char* vertexCode, const char* fragmentCode, int xx);

	void Compile();

	void UseSelf()const;

	void Delete()const;

	void AddIncludeFile(const std::string& filePath);

	void SetBool(const std::string& name, bool value) const;

	void SetInt(const std::string& name, int value) const;

	void SetFloat(const std::string& name, float value) const;

	void SetVec2(const std::string& name, const glm::vec2& value) const;

	void SetVec2(const std::string& name, float x, float y) const;

	void SetVec3(const std::string& name, const glm::vec3& value) const;

	void SetVec3(const std::string& name, float x, float y, float z) const;

	void SetVec4(const std::string& name, const glm::vec4& value) const;

	void SetVec4(const std::string& name, float x, float y, float z, float w) const;

	void SetMat2(const std::string& name, const glm::mat2& mat) const;

	void SetMat3(const std::string& name, const glm::mat3& mat) const;

	void SetMat4(const std::string& name, const glm::mat4& mat) const;

	template<typename T>
	void SetUniform(const std::string& name, T value, void(*pf)(const std::string&, T));

	void BlockBindingUniform(unsigned int pos, const char* uniform) const;


	static std::string FILE_HEAD;

	unsigned int ID;

private:

	void CheckCompileErrors(GLuint shader, std::string type)const;


	bool hasGeometry;

	std::string vertexShader;

	std::string fragmentShader;

	std::string geometryShader;
};

template<typename T>
inline void ShaderCompiler::SetUniform(const std::string& name, T value, void(*pf)(const std::string&, T))
{
	pf(name, value);
}

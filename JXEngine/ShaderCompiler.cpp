#include "ShaderCompiler.h"

#include <fstream>
#include <sstream>
#include <iostream>

std::string ShaderCompiler::FILE_HEAD = "#version 430 core";

ShaderCompiler::ShaderCompiler(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	hasGeometry = false;

	// 1. retrieve the vertex/fragment source code from filePath
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		
		// convert stream into string
		vertexShader = vShaderStream.str();
		fragmentShader = fShaderStream.str();
		
		// if geometry shader path is present, also load a geometry shader
		if (geometryPath != nullptr)
		{
			hasGeometry = true;
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryShader = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
}

ShaderCompiler::ShaderCompiler(const char* vertexCode, const char* fragmentCode, int xx)
{
	hasGeometry = false;

	vertexShader = vertexCode;

	fragmentShader = fragmentCode;
}

ShaderCompiler::ShaderCompiler(const char* vertexCode, const char* fragmentCode, const char* geometryCode, int xx)
{
	vertexShader = vertexCode;

	geometryShader = geometryCode;

	fragmentShader = fragmentCode;
}

ShaderCompiler::ShaderCompiler(const ShaderCompiler& other)
{
	vertexShader = other.vertexShader;
	geometryShader = other.geometryShader;
	hasGeometry = other.hasGeometry;
	fragmentShader = other.fragmentShader;
	
}

void ShaderCompiler::Compile()
{
	// 2. compile shaders

	const char* vShaderCode = vertexShader.c_str();
	const char* fShaderCode = fragmentShader.c_str();

	unsigned int vertex, fragment;

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");

	// if geometry shader is given, compile geometry shader
	unsigned int geometry;
	if (hasGeometry)
	{
		const char* gShaderCode = geometryShader.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		CheckCompileErrors(geometry, "GEOMETRY");
	}

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (hasGeometry)
		glAttachShader(ID, geometry);
	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (hasGeometry)
		glDeleteShader(geometry);
}

void ShaderCompiler::Delete() const
{
	glDeleteProgram(ID);

}

void ShaderCompiler::AddIncludeFile(const std::string& filePath)
{
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	
	try
	{
		file.open(filePath);
		std::stringstream content;
		content << file.rdbuf();
		file.close();
		
		fragmentShader = std::string(fragmentShader, fragmentShader.find("\n"));
		fragmentShader = content.str() + "\n" + fragmentShader;
		fragmentShader = FILE_HEAD + "\n" + fragmentShader;

	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

}

void ShaderCompiler::SetBool(const std::string& name, bool value) const
{
	UseSelf();
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ShaderCompiler::SetInt(const std::string& name, int value) const
{
	UseSelf();
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderCompiler::SetFloat(const std::string& name, float value) const
{
	UseSelf();
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderCompiler::SetVec2(const std::string& name, const glm::vec2& value) const
{
	UseSelf();
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ShaderCompiler::SetVec2(const std::string& name, float x, float y) const
{
	UseSelf();
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void ShaderCompiler::SetVec3(const std::string& name, const glm::vec3& value) const
{
	UseSelf();
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ShaderCompiler::SetVec3(const std::string& name, float x, float y, float z) const
{
	UseSelf();
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void ShaderCompiler::SetVec4(const std::string& name, const glm::vec4& value) const
{
	UseSelf();
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ShaderCompiler::SetVec4(const std::string& name, float x, float y, float z, float w) const
{
	UseSelf();
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void ShaderCompiler::SetMat2(const std::string& name, const glm::mat2& mat) const
{
	UseSelf();
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderCompiler::SetMat3(const std::string& name, const glm::mat3& mat) const
{
	UseSelf();
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderCompiler::SetMat4(const std::string& name, const glm::mat4& mat) const
{
	UseSelf();
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderCompiler::BlockBindingUniform(unsigned int pos, const char* uniform) const
{
	unsigned int id = glGetUniformBlockIndex(ID, uniform);
	glUniformBlockBinding(ID, id, pos);
}

ShaderCompiler ShaderCompiler::Copy()
{
	ShaderCompiler temp = *this;
	temp.Compile();
	return temp;;
}



void ShaderCompiler::UseSelf() const
{
	glUseProgram(ID);
}

void ShaderCompiler::CheckCompileErrors(GLuint shader, std::string type) const
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::ofstream outFile("1.txt");
			outFile << fragmentShader;
			outFile.close();
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

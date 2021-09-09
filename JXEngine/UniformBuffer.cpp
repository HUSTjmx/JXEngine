#include <iostream>
#include "UniformBuffer.h"
#include "Tools.h"

// According to the type of T, return the size of Block.
unsigned int GetSizeByType(UniformDataType type_)
{
	switch (type_)
	{
	case UniformDataType::Float:
		return BufferTool::Instance().GetBaseAlignment_std140<float, 1>();
		break;
	case UniformDataType::Vector3:
		return BufferTool::Instance().GetBaseAlignment_std140<glm::vec3, 1>();
		break;
	case UniformDataType::Mat4:
		return BufferTool::Instance().GetBaseAlignment_std140<glm::vec3, 4>();
		break;
	case UniformDataType::Bool:
		return BufferTool::Instance().GetBaseAlignment_std140<bool, 1>();
		break;
	case UniformDataType::Int:
		return BufferTool::Instance().GetBaseAlignment_std140<int, 1>();
		break;
	default:
		break;
	}
}

void UniformBuffer::Create()
{
	glGenBuffers(1, &ubo);
	//std::cout << size << std::endl;
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::LinkBindingPoint(unsigned int pos, unsigned int offset)
{
	glBindBufferRange(GL_UNIFORM_BUFFER, pos, ubo, offset, size);
}

void UniformBuffer::AddData(UniformData data)
{
	auto i = GetSizeByType(data.type);
	data.size = i;
	data.offset = size;
	size += i;
	datas.push_back(data);
	//std::cout << size << std::endl;
}

std::string UniformBuffer::GetName() const
{
	return name;
}

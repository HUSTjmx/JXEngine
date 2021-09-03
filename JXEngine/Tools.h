#pragma once
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//namespace StructureTool {
//	template<class T, size_t N >
//	constexpr size_t GetArrayCount(T(&arr)[N]) noexcept
//	{
//		return N;
//	}
//}

class StringTool
{

public:

	StringTool& Instance()
	{
		static StringTool* instance = new StringTool();
		return *instance;
	}

	std::string DelLineFromFile(std::ifstream& file, unsigned int num);

private:
	StringTool();
};

extern void SortPosByDistance(std::vector<glm::vec3>& v, glm::vec3 c_pos);



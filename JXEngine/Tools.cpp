#include "Tools.h"

std::string StringTool::DelLineFromFile(std::ifstream& file, unsigned int num)
{
	char temp[1024] = { 0 };
	std::string result;
	int c_num = 10;
	while (file.getline(temp, sizeof(temp)))
	{
		if (c_num == num)
		{
			result += "";
		}
		else {
			result += temp;
			result += "";
		}
		c_num++;
	}
	return result;
}

void SortPosByDistance(std::vector<glm::vec3>& v, glm::vec3 c_pos)
{
	auto func = [c_pos](glm::vec3 a, glm::vec3 b)
	{
		return glm::length(c_pos - a) > glm::length(c_pos - b);
	};

	std::sort(v.begin(), v.end(), func);
	//std::sort(v.begin(), v.end(),)
}

glm::mat4 MatTool::RemoveTranslation(glm::mat4 a)
{
	return glm::mat4(glm::mat3(a));
}

#include "Tools.h"
#include <iostream>

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

void SphereCreator::Create_ICO(std::vector<float>& v_d, std::vector<unsigned int>& indexs, std::vector<unsigned int>& offsets, int recursionLevel)
{
	m_cache.clear();
	vertexs.clear();

	index = 0;

	offsets.push_back(3);
	offsets.push_back(3);
	offsets.push_back(2);

	auto t = (1.0f + std::sqrt(5.0f)) / 2.0f;

	AddVertex(glm::vec3(-1, t, 0));
	AddVertex(glm::vec3(1, t, 0));
	AddVertex(glm::vec3(-1, -t, 0));
	AddVertex(glm::vec3(1, -t, 0));

	AddVertex(glm::vec3(0, -1, t));
	AddVertex(glm::vec3(0, 1, t));
	AddVertex(glm::vec3(0, -1, -t));
	AddVertex(glm::vec3(0, 1, -t));
	

	AddVertex(glm::vec3(t, 0, -1));
	AddVertex(glm::vec3(t, 0, 1));
	AddVertex(glm::vec3(-t, 0, -1));
	AddVertex(glm::vec3(-t, 0, 1));

	std::vector<face> faces;

	// 1
	faces.push_back(face(0, 11, 5));
	faces.push_back(face(0, 5, 1));
	faces.push_back(face(0, 1, 7));
	faces.push_back(face(0, 7, 10));
	faces.push_back(face(0, 10, 11));

	// 2
	faces.push_back(face(1, 5, 9));
	faces.push_back(face(5, 11, 4));
	faces.push_back(face(11, 10, 2));
	faces.push_back(face(10, 7, 6));
	faces.push_back(face(7, 1, 8));

	// 3
	faces.push_back(face(3, 9, 4));
	faces.push_back(face(3, 4, 2));
	faces.push_back(face(3, 2, 6));
	faces.push_back(face(3, 6, 8));
	faces.push_back(face(3, 8, 9));

	// 4
	faces.push_back(face(4, 9, 5));
	faces.push_back(face(2, 4, 11));
	faces.push_back(face(6, 2, 10));
	faces.push_back(face(8, 6, 7));
	faces.push_back(face(9, 8, 1));

	for (int i = 0; i < recursionLevel; ++i)
	{
		std::vector<face> faces2;

		for (auto v = faces.begin(); v != faces.end(); ++v)
		{
			int a = GetMiddlePoint(v->v1, v->v2);
			int b = GetMiddlePoint(v->v2, v->v3);
			int c = GetMiddlePoint(v->v3, v->v1);

			faces2.push_back(face(v->v1, a, c));
			faces2.push_back(face(v->v2, b, a));
			faces2.push_back(face(v->v3, c, b));
			faces2.push_back(face(a, b, c));
		}
		faces = faces2;
	}

	for (auto f = faces.begin(); f != faces.end(); ++f)
	{
		indexs.push_back(f->v1);
		indexs.push_back(f->v2);
		indexs.push_back(f->v3);

		/*v_d.push_back(vertexs[f->v1].pos.x);
		v_d.push_back(vertexs[f->v1].pos.y);
		v_d.push_back(vertexs[f->v1].pos.z);
		v_d.push_back(vertexs[f->v1].normal.x);
		v_d.push_back(vertexs[f->v1].normal.y);
		v_d.push_back(vertexs[f->v1].normal.z);
		v_d.push_back(vertexs[f->v1].uv.x);
		v_d.push_back(vertexs[f->v1].uv.y);

		v_d.push_back(vertexs[f->v2].pos.x);
		v_d.push_back(vertexs[f->v2].pos.y);
		v_d.push_back(vertexs[f->v2].pos.z);
		v_d.push_back(vertexs[f->v2].normal.x);
		v_d.push_back(vertexs[f->v2].normal.y);
		v_d.push_back(vertexs[f->v2].normal.z);
		v_d.push_back(vertexs[f->v2].uv.x);
		v_d.push_back(vertexs[f->v2].uv.y);

		v_d.push_back(vertexs[f->v3].pos.x);
		v_d.push_back(vertexs[f->v3].pos.y);
		v_d.push_back(vertexs[f->v3].pos.z);
		v_d.push_back(vertexs[f->v3].normal.x);
		v_d.push_back(vertexs[f->v3].normal.y);
		v_d.push_back(vertexs[f->v3].normal.z);
		v_d.push_back(vertexs[f->v3].uv.x);
		v_d.push_back(vertexs[f->v3].uv.y);*/

	}

	for (auto v = vertexs.begin(); v != vertexs.end(); ++v)
	{
		//std::cout << v->pos.x << " " << v->pos.y << " " << v->pos.z << std::endl;
		v_d.push_back(v->pos.x);
		v_d.push_back(v->pos.y);
		v_d.push_back(v->pos.z);
		v_d.push_back(v->normal.x);
		v_d.push_back(v->normal.y);
		v_d.push_back(v->normal.z);
		v_d.push_back(v->uv.x);
		v_d.push_back(v->uv.y);
	}

}

int SphereCreator::AddVertex(glm::vec3 pos)
{
	Vertex v;
	pos = glm::normalize(pos);

	v.pos = pos;
	v.normal = pos;
	v.uv = glm::vec2(0.5f + pos.x / 2.0, 0.5f + pos.y / 2.0);

	vertexs.push_back(v);
	return index++;
}

int SphereCreator::GetMiddlePoint(int p1, int p2)
{
	bool firstIsSmaller = p1 < p2;
	_int64 smallerIndex = firstIsSmaller ? p1 : p2;
	_int64 greaterIndex = firstIsSmaller ? p2 : p1;
	_int64 key = (smallerIndex << 32) + greaterIndex;

	auto re = m_cache.find(key);
	if (re != m_cache.end())
	{
		return re->second;
	}

	Vertex point1 = vertexs[p1];
	Vertex point2 = vertexs[p2];

	glm::vec3 mid = (point1.pos + point2.pos) / 2.0f;
	//std::cout << point1.pos.x << " " << point2.pos.x << "  " << mid.x << std::endl;
	int i = AddVertex(mid);

	m_cache[key] = i;

	return i;
}

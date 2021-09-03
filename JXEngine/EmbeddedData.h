#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

namespace LEARN_OPENGL {
	//�����ζ�������
	extern float triangleVertices[];

	//���ζ�������
	 extern float quadVertices[];

	//������������
	 extern unsigned int quadIndices[];

	//����ƫ������
	 extern unsigned int quadOffsets[];
}

namespace LEARN_OPENGL_VERTICE {
	namespace START_01 {
		extern float Cube_vertices[288];

		extern unsigned int Cube_Offsets[3];
	}

	namespace HIGH_03 {
		extern float planeVertices[30];

		extern float transparentVertices[30];

		extern unsigned int offsets[2];
	}
}

namespace LEARN_OPENGL_POSITION {
	namespace Start_01 {
		extern std::vector<glm::vec3> cubePositions;
	}
}

namespace LEARN_OPENGL_TEXTURE {

	namespace START_01 {
		extern std::string Start_01_container;

		extern std::string Start_01_wall;
	}

	namespace HIGH_03
	{
		extern std::string High_03_Grass;

		extern std::string High_03_window;
	}


}

namespace LEARN_OPENGL_SHADER
{
	extern std::string Start_01_texture_vs;

	 extern std::string Start_01_texture_fs;

	 extern std::string Start_02_camera_vs;

	 extern std::string Start_02_camera_fs;

	 extern std::string Light_02_lightCube_vs;

	 extern std::string Light_02_lightCube_fs;

	 extern std::string Light_02_lightTest_vs;

	 extern std::string Light_02_lightTest_fs;

	 extern std::string High_03_AlphaTest_vs;

	 extern std::string High_03_AlphaTest_fs;

	namespace TEXTURE_NAME {
		extern std::string texture_name1;

		extern std::string texture_name2;
	}
}

namespace ASSETS
{
	namespace MODEL
	{
		extern std::string MODEL_NANOSUIT;
	}
}
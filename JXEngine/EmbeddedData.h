#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

namespace LEARN_OPENGL {
	//三角形顶点数据
	extern float triangleVertices[];

	//方形顶点数据
	 extern float quadVertices[];

	//方形索引数据
	 extern unsigned int quadIndices[];

	//方形偏移数组
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

		extern float QuadVertices[24];

		extern unsigned int QuadOffsets[2];

		extern float CubeMapVertice[108];

		extern unsigned int CubeMapOffsets[1];
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

		extern std::vector<std::string> skybox1_face;
	}


}

namespace LEARN_OPENGL_SHADER
{
	extern std::string address;

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

	 extern std::string High_04_FrameTest_vs;

	 extern std::string High_04_FrameTest_fs;

	 extern std::string High_05_CubeMapTest_vs;

	 extern std::string High_05_CubeMapTest_fs;

	 extern std::string High_06_SkyBox_vs;

	 extern std::string High_06_SkyBox_fs;
	 
	 extern std::string High_07_UniformBuffer_vs;
	 
	 extern std::string High_07_UniformBuffer_fs;

	 extern std::string High_08_GeometryNormal_vs;

	 extern std::string High_08_GeometryNormal_fs;

	 extern std::string High_08_GeometryNormal_gs;

	 extern std::string High_09_InstanceTest_ast_vs;

	 extern std::string High_09_InstanceTest_ast_fs;

	 extern std::string High_09_InstanceTest_pla_vs;

	 extern std::string High_09_InstanceTest_pla_fs;

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

		extern std::string MODEL_ASTERIOD;

		extern std::string MODEL_PLANET;
	}
}
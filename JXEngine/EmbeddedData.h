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
		extern std::vector<float> Cube_vertices;

		extern std::vector<unsigned int> Cube_Offsets;
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

	namespace HIGH_LIGHT_04
	{
		extern float PlaneVertices[48];

		extern unsigned int PlaneOffsets[3];
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

	 extern std::string HighLight_10_ShadowMapGet_vs;

	 extern std::string HighLight_10_ShadowMapGet_fs;

	 extern std::string HighLight_10_ShadowMapDebug_vs;

	 extern std::string HighLight_10_ShadowMapDebug_fs;

	 extern std::string HighLight_11_BaseShadow_vs;

	 extern std::string HighLight_11_BaseShadow_fs;

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

	namespace TEXTURE
	{
		namespace NOISY
		{
			extern std::string Path;
			extern std::string Star;

			// Gray Noise.
			// ...
			extern std::string Gray_Noise_Small;
			extern std::string Gray_Noise_Medium;
		}
	}
}

namespace SHADER_PATH
{
	namespace RAY_MARCHING
	{
		namespace SKY
		{
			extern std::string Path;
			extern std::string earthSky_vs;
			extern std::string earthSky_fs;
		}

		namespace CLOUD
		{
			extern std::string Path;
			extern std::string Cloud_01_vs;
			extern std::string Cloud_01_fs;

			extern std::string Cloud_02_vs;
			extern std::string Cloud_02_fs;
		}
	}

	namespace POST_RENDER
	{
		namespace FOVEA
		{
			extern std::string Path;
			extern std::string KernelFovea_vs;
			extern std::string KernelFovea_fs;
			extern std::string KernelFovea_Pass2_fs;
		}
	}
}
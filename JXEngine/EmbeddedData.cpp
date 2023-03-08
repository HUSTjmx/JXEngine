#include "EmbeddedData.h"

namespace LEARN_OPENGL {
	//三角形顶点数据
	 float triangleVertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	//方形顶点数据
	 float quadVertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	//方形索引数据
	 unsigned int quadIndices[] = { // 注意索引从0开始! 
		0, 1, 3, // 第一个三角形
		1, 2, 3  // 第二个三角形
	};

	//方形偏移数组
	 unsigned int quadOffsets[] = { 3, 3, 2 };
}

namespace LEARN_OPENGL_VERTICE {
	namespace START_01 {
		std::vector<float>  Cube_vertices = {
			// Back face
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
			// Front face
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
			// Left face
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// Right face
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
			// Bottom face
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// Top face
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left   
		};

		std::vector<unsigned int>  Cube_Offsets = { 3, 3, 2 };
	}

	namespace HIGH_03 {
		float planeVertices[30] = {
			// positions          // texture Coords 
			 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
			-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

			 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
			 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
		};

		float transparentVertices[30] = {
			// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

			0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
			1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
			1.0f,  0.5f,  0.0f,  1.0f,  0.0f
		};

		unsigned int offsets[2] = { 3, 2 };

		float QuadVertices[24] =
		{
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};

		unsigned int QuadOffsets[2] = { 2, 2 };

		float CubeMapVertice[108] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		unsigned int CubeMapOffsets[1] = { 3 };
	}

	namespace HIGH_LIGHT_04
	{
		float PlaneVertices[48] = {
			// Positions          // Normals         // Texture Coords
			25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
			-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
			-25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

			25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
			25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
			-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f
		};

		unsigned int PlaneOffsets[3] = { 3, 3, 2 };
	}
}

namespace LEARN_OPENGL_POSITION {
	namespace Start_01 {
		 std::vector<glm::vec3> cubePositions = {
			//glm::vec3(0.0f,  0.0f,  0.0f),
			//glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};
	}
}

namespace LEARN_OPENGL_TEXTURE {

	namespace START_01 {
		 std::string Start_01_container = "resources/LearnOpenGL/Start_01/container.jpg";

		 std::string Start_01_wall = "resources/LearnOpenGL/Start_01/wall.jpg";
	}

	namespace HIGH_03
	{
		std::string High_03_Grass = "resources/LearnOpenGL/High_03/grass.png";

		std::string High_03_window = "resources/LearnOpenGL/High_03/blending_transparent_window.png";

		std::vector<std::string> skybox1_face =
		{
			"resources/textures/skybox1/right.jpg",
			"resources/textures/skybox1/left.jpg",
			"resources/textures/skybox1/top.jpg",
			"resources/textures/skybox1/bottom.jpg",
			"resources/textures/skybox1/front.jpg",
			"resources/textures/skybox1/back.jpg",
		};
	}
}

namespace LEARN_OPENGL_SHADER
{
	std::string address = "shaders/";

	 std::string Start_01_texture_vs = address + "texture_01.vs";

	 std::string Start_01_texture_fs = address + "texture_01.fs";

	 std::string Start_02_camera_vs = address + "camera_02.vs";

	 std::string Start_02_camera_fs = address + "camera_02.fs";

	 std::string Light_02_lightCube_vs = address + "lightCube.vs";

	 std::string Light_02_lightCube_fs = address + "lightCube.fs";

	 std::string Light_02_lightTest_vs = address + "lightTest.vs";

	 std::string Light_02_lightTest_fs = address + "lightTest.fs";

	 std::string High_03_AlphaTest_vs = address + "AlphaTest_01.vs";

	 std::string High_03_AlphaTest_fs = address + "AlphaTest_01.fs";

	 std::string High_04_FrameTest_vs = address + "FrameBufferTest_02.vs";

	 std::string High_04_FrameTest_fs = address + "FrameBufferTest_02.fs";

	 std::string High_05_CubeMapTest_vs = address + "CubeMap_03.vs";

	 std::string High_05_CubeMapTest_fs = address + "CubeMap_03.fs";

	 std::string High_06_SkyBox_vs = address + "skybox_04.vs";

	 std::string High_06_SkyBox_fs = address + "skybox_04.fs";

	 std::string High_07_UniformBuffer_vs = address + "UniformBufferTest_05.vs";
	 
	 std::string High_07_UniformBuffer_fs = address + "UniformBufferTest_05.fs";

	 std::string High_08_GeometryNormal_vs = address + "GeometryNormalV_06.vs";

	 std::string High_08_GeometryNormal_fs = address + "GeometryNormalV_06.fs";

	 std::string High_08_GeometryNormal_gs = address + "GeometryNormalV_06.gs";

	 std::string High_09_InstanceTest_ast_vs = address + "High03_InstanceTest_asteroids.vs";

	 std::string High_09_InstanceTest_ast_fs = address + "High03_InstanceTest_asteroids.fs";

	 std::string High_09_InstanceTest_pla_vs = address + "High03_InstanceTest_planet.vs";

	 std::string High_09_InstanceTest_pla_fs = address + "High03_InstanceTest_planet.fs";

	 std::string HighLight_10_ShadowMapGet_vs = address + "HighLight04_ShadowMapGet.vs";

	 std::string HighLight_10_ShadowMapGet_fs = address + "HighLight04_ShadowMapGet.fs";

	 std::string HighLight_10_ShadowMapDebug_vs = address + "HighLight04_ShadowMapDebug.vs";

	 std::string HighLight_10_ShadowMapDebug_fs = address + "HighLight04_ShadowMapDebug.fs";

	 std::string HighLight_11_BaseShadow_vs = address + "HighLight04_BaseShadow_04.vs";
	  
	 std::string HighLight_11_BaseShadow_fs = address + "HighLight04_BaseShadow_04.fs";

	namespace TEXTURE_NAME {
		 std::string texture_name1 = "texture1";

		 std::string texture_name2 = "texture2";
	}
}


namespace ASSETS
{
	namespace MODEL
	{
		std::string MODEL_NANOSUIT = "resources/model/nanosuit/nanosuit.obj";

	    std::string MODEL_ASTERIOD = "resources/model/rock/rock.obj";

		std::string MODEL_PLANET = "resources/model/planet/planet.obj";
	}

	namespace TEXTURE
	{
		namespace NOISY
		{
			std::string Path = "resources/textures/noisy/";
			std::string Star = Path + "star.jpg";
			std::string Gray_Noise_Small = Path + "GrayNoise_Small.png";
			std::string Gray_Noise_Medium = Path + "GrayNoise_Medium.png";
		}
	}
}

namespace SHADER_PATH
{
	namespace RAY_MARCHING
	{
		namespace SKY
		{
			std::string Path = "shaders/RayMarching/Sky/";
			std::string earthSky_vs = Path + "sky.vs";
			std::string earthSky_fs = Path + "sky.fs";
		}

		namespace MEDIA
		{
			std::string Path = "shaders/RayMarching/MediaRendering/";
			std::string Cloud_01_vs = Path + "Cloud_01.vs";
			std::string Cloud_01_fs = Path + "Cloud_01.fs";

			std::string Cloud_02_vs = Path + "CloudFloor_01.vs";
			std::string Cloud_02_fs = Path + "CloudFloor_01.fs";

			std::string PlayingMarble_vs = Path + "PlayingMarble.vs";
			std::string PlayingMarble_fs = Path + "PlayingMarble.fs";
		}

		namespace PURE_CLOUD
		{
			std::string Path = "shaders/RayMarching/Cloud/";
			std::string Cloud_01_vs = Path + "Cloud_1.vs";
			std::string Cloud_01_fs = Path + "Cloud_1.fs";

			std::string Cloud_02_vs = Path + "Cloud_2.vs";
			std::string Cloud_02_fs = Path + "Cloud_2.fs";
		}

		namespace FBM_GREAT_EFFECT
		{
			std::string Path = "shaders/RayMarching/FBM_GreatEffect/";
			std::string MysteryMoutains_vs = Path + "MysteryMountains.vs";
		    std::string MysteryMoutains_fs = Path + "MysteryMountains.fs";
		}

		namespace PAPER
		{
			std::string Path = "shaders/RayMarching/Paper/";
			std::string StaticScene_01_vs = Path + "StaticScene_01.vs";
			std::string StaticScene_01_fs = Path + "StaticScene_01.fs";

			std::string StaticScene_02_vs = Path + "StaticScene_02.vs";
			std::string StaticScene_02_fs = Path + "StaticScene_02.fs";
		}
	}

	namespace POST_RENDER
	{
		namespace FOVEA
		{
			std::string Path = "shaders/PostRendering/FoveatedRender/";
			std::string KernelFovea_vs = Path + "KernelFovea.vs";
			std::string KernelFovea_fs = Path + "KernelFovea.fs";
			std::string KernelFovea_Pass2_fs = Path + "KernelFovea_pass2.fs";
		}

		namespace BLUR
		{
			std::string Path = "shaders/PostRendering/Blur/";
			std::string FoveatedBlur_vs = Path + "FoveatedBlur.vs";
			std::string FoveatedBlur_fs = Path + "FoveatedBlur.fs";
		}
	}

	namespace PAPER
	{
		std::string Path = "shaders/Paper/";

		// Paper Scene 01 : Sky atmosphere
		std::string PaperScene_01_Sky_VS = Path + "PaperScene_01_Sky.vs";
		std::string PaperScene_01_Sky_FS = Path + "PaperScene_01_Sky.fs";

		// Paper Scene 02 : Fog Ball
		std::string PaperScene_02_FogBall_VS = Path + "PaperScene_02_FogBall.vs";
		std::string PaperScene_02_FogBall_01_N_FS = Path + "PaperScene_02_FogBall_01_N.fs";
		std::string PaperScene_02_FogBall_01_T_FS = Path + "PaperScene_02_FogBall_01_T.fs";
		std::string PaperScene_02_FogBall_02_N_FS = Path + "PaperScene_02_FogBall_02_N.fs";

		// Paper Scene 03 : Stanford Rabbits
		std::string PaperScene_03_StanfordRabbits_VS = Path + "PaperScene_03_StanfordRabbits.vs";
		std::string PaperScene_03_StanfordRabbits_01_N_FS = Path + "PaperScene_03_StanfordRabbits_01_N.fs";
		std::string PaperScene_03_StanfordRabbits_01_T_FS = Path + "PaperScene_03_StanfordRabbits_01_T.fs";
	
	}
}
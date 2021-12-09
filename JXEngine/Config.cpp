#include "Config.h"
#include "Camera.h"
namespace CONFIG
{

	namespace SCREEN_CONFIG
	{
		unsigned int SCR_WIDTH = 1000;
		unsigned int SCR_HEIGHT = 800;

		namespace FOVEA
		{
			unsigned int FOVEA_WIDTH = 999;
			unsigned int FOVEA_HEIGHT = 799;
		}
	}

	namespace CAMERA_CONFIG
	{
		float NEAR_PLANE = 0.6f;
		float FAR_PLANE = 200.0f;
		float ASPECT = (float)SCREEN_CONFIG::SCR_WIDTH / (float)SCREEN_CONFIG::SCR_HEIGHT;
		
		namespace SHADER {
			std::string VIEW_POS_IN_WORLD = "viewPosWS";
			std::string CENTER_DIR = "view_center_dir";
			std::string FAR_PLANE = "far_plane";
			std::string NEAR_PLANE = "near_plane";
		}
	}

	namespace SHADOW_MAP
	{
		unsigned int SHADOW_WIDTH = 1024;
		unsigned int SHADOW_HEIGHT = 1024;

		Camera DIR_LIGHT_CAMERA;

		glm::mat4 LIGHT_SPACE_MAT;

		namespace SHADER_NAME {
			std::string LIGHT_SPACE_MAT_NAME = "lightSpaceMatrix";

			std::string SHADOW_MAP_NAME = "shadowMap";
		}
	}

	namespace SHADER_DEFAULT_UNIFORM_NAME
	{
		const std::string MODEL_MATRIX = "model";
		const std::string PROJECTION_MATRIX = "projection";
		const std::string PROJECTION_MATRIX_INV = "projection_inv";
		const std::string PROJECTION_MATRIX_PRE = "projection_pre";
		const std::string VIEW_MATRIX = "view";
		const std::string VIEW_MATRIX_INV = "view_inv";
		const std::string VIEW_MATRIX_PRE = "view_pre";

		const std::string TIME = "iTime";

		const std::string RESOLUTION = "Resolution";

		namespace UNIFORM_BLOCK_NAME
		{
			const std::string MATRIX_COORD_SYSTEM = "Matrices";
		}
	}

	namespace LIGHT_SETTINGS
	{
		 unsigned int MAX_DIRECTION_LIGHT_NUM = 1;
		 unsigned int MAX_POINT_LIGHT_NUM = 4;
		 unsigned int MAX_SPOT_LIGHT_NUM = 4;

		namespace SHADER_DIR_LIGHT
		{
			const std::string NUM_STRING = "dirLightsNum";
			const std::string MAIN_NAME = "dirLights";
			const std::string COLOR = "color";
			const std::string DIRECTION = "direction";
			const std::string INTENSITY = "intensity";
		}

		namespace SHADER_POINT_LIGHT
		{
			const std::string NUM_STRING = "pointLightsNum";
			const std::string MAIN_NAME = "pointLights";
			const std::string COLOR = "color";
			const std::string POSITION = "position";
			const std::string INTENSITY = "intensity";
			const std::string LIGHT_IN_RADIUS = "lightInvRadius";

			std::vector<glm::vec3> POSITION_ARRAY = {
				glm::vec3(0.7f,  0.2f,  2.0f),
				glm::vec3(2.3f, -3.3f, -4.0f),
				glm::vec3(-4.0f,  2.0f, -12.0f),
				glm::vec3(0.0f,  0.0f, -3.0f)
			};

			std::vector<glm::vec3> COLOR_ARRAY =
			{
				glm::vec3(1.0, 1.0, 1.0),
				glm::vec3(0.0, 1.0, 1.0),
				glm::vec3(1.0, 0.0, 1.0),
				glm::vec3(1.0, 1.0, 0.0)
			};

			std::vector<float> RADIUS_ARRAY = {
				10.0f,
				10.0f,
				10.0f,
				10.0f
			};
		}

		namespace SHADER_SPOT_LIGHT
		{
			const std::string NUM_STRING = "spotLightsNum";
			const std::string MAIN_NAME = "spotLights";
			const std::string COLOR = "color";
			const std::string POSITION = "position";
			const std::string DIRECTION = "direction";
			const std::string INTENSITY = "intensity";
			const std::string LIGHT_IN_RADIUS = "lightInvRadius";
			const std::string INNER_ANGLE = "innerAngle";
			const std::string OUTER_ANGLE = "outerAngle";
		}
	}

	namespace MATERIAL_SETTINGS
	{
		namespace STANDORD_MODEL
		{
			const std::string BASE_COLOR = "baseColor";
			const std::string METALLIC = "metallic";
			const std::string ROUGHNESS = "roughness";
			const std::string REFLECTANCE = "reflectance";
			const std::string EMISSIVE = "emissive";
		}

		namespace SKY_MODEL
		{
			const std::string EARTH_RADIUS = "earthRadius";
		    const std::string ATMOSHERE_RADIUS = "atmosphereRadius";
			const std::string HR = "Hr";
			const std::string HM = "Hm";
		    const std::string BETA_R = "betaR";
			const std::string BETA_M = "betaM";
		}

		namespace FOVEA_RENDER
		{
			std::string RESOLUTION_NAME = "LowResolution";
		}
	}

	namespace SHADING_INCLUDE_CORE
	{
		const std::string address = "shaders/Core/";
		const std::string MATH = address + "Math.cg";
		const std::string BRDF = address + "BRDF.cg";
		const std::string LIGHT = address + "Light.cg";
		const std::string UNIFORM = address + "Uniform.cg";
		const std::string SHADOW = address + "Shadow.cg";
		const std::string RAY_MARCHING = address + "RayMarching.cg";
		const std::string POST_PROCESSING = address + "Post.cg";
		const std::string COLOR = address + "Color.cg"; 
	}
}

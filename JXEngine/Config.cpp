#include "Config.h"
namespace CONFIG
{

	namespace SCREEN_CONFIG
	{
		unsigned int SCR_WIDTH = 800;
		unsigned int SCR_HEIGHT = 600;
	}

	namespace CAMERA_CONFIG
	{
		float NEAR_PLANE = 0.1f;
		float FAR_PLANE = 100.0f;
		float ASPECT = (float)SCREEN_CONFIG::SCR_WIDTH / (float)SCREEN_CONFIG::SCR_HEIGHT;
		
		namespace SHADER {
			std::string VIEW_POS_IN_WORLD = "ViewPosWS";
		}
	}

	namespace SHADER_DEFAULT_UNIFORM_NAME
	{
		const std::string MODEL_MATRIX = "model";
		const std::string PROJECTION_MATRIX = "projection";
		const std::string VIEW_MATRIX = "view";
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
			const std::string LIGHT_IN_RADIUS = "lightInRadius";

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
			const std::string LIGHT_IN_RADIUS = "lightInRadius";
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
	}

	namespace SHADING_INCLUDE_CORE
	{
		const std::string MATH = "Math.cg";
		const std::string BRDF = "BRDF.cg";
		const std::string LIGHT = "Light.cg";
		const std::string UNIFORM = "Uniform.cg";
	}
}

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

class Camera;

namespace CONFIG
{

	namespace SCREEN_CONFIG
	{
		extern unsigned int SCR_WIDTH;
		extern unsigned int SCR_HEIGHT;

		namespace FOVEA
		{
			extern unsigned int FOVEA_WIDTH;
			extern unsigned int FOVEA_HEIGHT;
		}
	}

	namespace CAMERA_CONFIG
	{
		extern float NEAR_PLANE;
		extern float FAR_PLANE;
		extern float ASPECT;
		
		namespace SHADER {
			extern std::string VIEW_POS_IN_WORLD;
			extern std::string CENTER_DIR;
			extern std::string FAR_PLANE;
			extern std::string NEAR_PLANE;

			extern std::string POSITION_LAST_FRAME;
			extern std::string CENTER_DIR_LAST_FRAME;
		}
	}

	namespace TIME_CONFIG
	{
		extern float AverageRange;
	}

	namespace SHADOW_MAP
	{
		extern unsigned int SHADOW_WIDTH;
		extern unsigned int SHADOW_HEIGHT;

		extern Camera DIR_LIGHT_CAMERA;

		extern glm::mat4 LIGHT_SPACE_MAT;

		namespace SHADER_NAME {
			extern std::string LIGHT_SPACE_MAT_NAME;

			extern std::string SHADOW_MAP_NAME;
		}
	}

	namespace SHADER_DEFAULT_UNIFORM_NAME
	{
		extern const std::string MODEL_MATRIX;
		extern const std::string PROJECTION_MATRIX;
		extern const std::string PROJECTION_MATRIX_INV;
		extern const std::string PROJECTION_MATRIX_PRE;
		extern const std::string VIEW_MATRIX;
		extern const std::string VIEW_MATRIX_INV;
		extern const std::string VIEW_MATRIX_PRE;

		extern const std::string TIME;

		extern const std::string RESOLUTION;

		namespace UNIFORM_BLOCK_NAME
		{
			extern const std::string MATRIX_COORD_SYSTEM;
		}
	}

	namespace LIGHT_SETTINGS
	{
		extern unsigned int MAX_DIRECTION_LIGHT_NUM;
		extern unsigned int MAX_POINT_LIGHT_NUM;
		extern unsigned int MAX_SPOT_LIGHT_NUM;

		namespace SHADER_DIR_LIGHT
		{
			extern const std::string NUM_STRING;
			extern const std::string MAIN_NAME;
			extern const std::string COLOR;
			extern const std::string DIRECTION;
			extern const std::string INTENSITY;
		}

		namespace SHADER_POINT_LIGHT
		{
			extern const std::string NUM_STRING;
			extern const std::string MAIN_NAME;
			extern const std::string COLOR;
			extern const std::string POSITION;
			extern const std::string INTENSITY;
			extern const std::string LIGHT_IN_RADIUS;

			extern std::vector<glm::vec3> POSITION_ARRAY;

			extern std::vector<glm::vec3> COLOR_ARRAY;

			extern std::vector<float> RADIUS_ARRAY;
		}

		namespace SHADER_SPOT_LIGHT
		{
			extern const std::string NUM_STRING;
			extern const std::string MAIN_NAME;
			extern const std::string COLOR;
			extern const std::string POSITION;
			extern const std::string DIRECTION;
			extern const std::string INTENSITY;
			extern const std::string LIGHT_IN_RADIUS;
			extern const std::string INNER_ANGLE;
			extern const std::string OUTER_ANGLE;
		}
	}

	namespace MATERIAL_SETTINGS
	{
		namespace STANDORD_MODEL
		{
			extern const std::string BASE_COLOR;
			extern const std::string METALLIC;
			extern const std::string ROUGHNESS;
			extern const std::string REFLECTANCE;
			extern const std::string EMISSIVE;
		}

		namespace SKY_MODEL
		{
			extern const std::string EARTH_RADIUS;
			extern const std::string ATMOSHERE_RADIUS;
			extern const std::string HR;
			extern const std::string HM;
			extern const std::string BETA_R;
			extern const std::string BETA_M;
		}

		namespace FOVEA_RENDER
		{
			extern std::string RESOLUTION_NAME;
		}
	}

	namespace SHADING_INCLUDE_CORE
	{
		extern const std::string address;
		extern const std::string MATH;
		extern const std::string BRDF;
		extern const std::string LIGHT;
		extern const std::string UNIFORM;
		extern const std::string SHADOW;
		extern const std::string RAY_MARCHING;
		extern const std::string POST_PROCESSING;
		extern const std::string COLOR;

	}
}


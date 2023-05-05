#include "TFRM_Algorithm.h"
#include "Camera.h"
#include <cmath>

#define PI 3.14

namespace Algorithm
{
	namespace TFRM
	{
		namespace UNIFORM_NAME
		{
			std::string TEMPORAL_ACCELERATION_MUL_NAME = "TEMPORAL_ACCELERATION_MUL";

			std::string FACTOR_DIS = "F_DIS";
			std::string FACTOR_ANG = "F_ANG";

			std::string MAX_SHADOW_STEP_NAME = "MAX_SHADOW_STEP_NUM";
			std::string MIN_SHADOW_STEP_NAME = "MIN_SHADOW_STEP_NUM";

		    std::string MAX_STEP_NAME = "MAX_STEP_NUM";
			std::string MIN_STEP_NAME = "MIN_STEP_NUM";
		}

		namespace Setting
		{
			float TEMPORAL_ACCELERATION_MUL = 2.0;

			float MAX_DISTANCE_IS_FOVEAL_REGION = 80.0f;
			float MIN_SIZE_IS_FOVEAL_REGION = 4;
			bool MEDIASIZE_IS_LEQ_LIGHTZSIZE = false;
			
			float F_DIS_SCALING = 1.0;
			float F_ANG_SCALING = 1.0;

			float MAX_SHADOW_STEP_NUM = 32;
			float MIN_SHADOW_STEP_NUM = 8;
			
			float MAX_STEP_NUM = 64.0;
			float MIN_STEP_NUM = 32.0;
		}

		namespace MathFunc
		{
			float expStep(float x, float k, float n)
			{
				return exp(-k * pow(x, n));
			}

			float gain(float x, float k)
			{
				const float a = 0.5 * pow(2.0 * ((x < 0.5) ? x : 1.0 - x), k);
				return (x < 0.5) ? a : 1.0 - a;
			}

			float almostIdentity(float x, float m, float n)
			{
				if (x > m) return x;
				const float a = 2.0 * n - m;
				const float b = 2.0 * m - 3.0 * n;
				const float t = x / m;
				return (a * t + b) * t * t + n;
			}
		}

		float HSV_PDF(float e)
		{
			const float M = 1.0;
			if (e <= 5.79)return 1.0 * M * 1.0;
			return 7.49 / pow(0.3 * e + 1.0, 2.0) * M;
		}

		float Map_HSV(float v)
		{
			return exp(1.0 - v) - 1.0;
		}

		bool IsFovealRegion(float theta, float dis, float size)
		{
			bool A = (theta * 180.0) / PI < 5.79;
			bool B = dis < Setting::MAX_DISTANCE_IS_FOVEAL_REGION;
			bool C = size > Setting::MIN_SIZE_IS_FOVEAL_REGION;

			return A;
		}

		void ComputeTFRM(float& F_dis, float& F_ang, std::shared_ptr<Camera> camera)
		{
			F_dis = length(camera->LastPosition - camera->Position);
			F_ang = 1.0 - abs(dot(camera->Front, camera->LastFront));

			F_dis = exp(MathFunc::gain(F_dis, 0.5) * Setting::F_DIS_SCALING);
			F_ang = exp(MathFunc::gain(F_ang, 0.5) * Setting::F_ANG_SCALING);
		}

	}
}
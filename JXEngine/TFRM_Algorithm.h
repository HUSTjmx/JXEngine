#pragma once
#include <memory>
#include <string>

extern class Camera;

namespace Algorithm
{
	namespace TFRM
	{
		namespace UNIFORM_NAME
		{
			extern std::string FACTOR_DIS;
			extern std::string FACTOR_ANG;
		}

		namespace Setting
		{
			extern float MAX_DISTANCE_IS_FOVEAL_REGION;
			extern float MIN_SIZE_IS_FOVEAL_REGION;
			extern bool MEDIASIZE_IS_LEQ_LIGHTZSIZE;

			extern float F_DIS_SCALING;
			extern float F_ANG_SCALING;
		}

		namespace MathFunc
		{
			extern float expStep(float x, float k, float n);

			extern float gain(float x, float k);

			extern float almostIdentity(float x, float m, float n);
		}

		extern bool IsFovealRegion(float theta, float L, float R);

		extern float HSV_PDF(float e);

		extern float Map_HSV(float v);

		extern void ComputeTFRM(float& F_dis, float& F_ang, std::shared_ptr<Camera> camera);
	}
}
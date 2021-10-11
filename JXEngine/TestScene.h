#pragma once
#include "std.h"

class Material;
class Scene;
class Pass;

namespace OPENGL_SCENE
{
	class TestScene
	{
	public:
		static TestScene& Instance()
		{
			static TestScene* instance = new TestScene();
			return *instance;
		}

		Scene GetScene_Start01();

		Scene GetScene_AlphaTest_02();

		Scene GetScene_AlphaBlend_03();

		Scene GetScene_FrameTest_04();

		Scene GetScene_CubeMapTest_05();

		Scene GetScene_UniformBufferTest_06();

		Scene GetScene_GeometryNormal_07();

		Scene GetScene_InstanceTest_08();

		Scene GetScene_ShadowMap_09();

		Scene GetScene_BackFaceDepth_09_01();

		Scene GetScene_FrontFaceDepth_09_02();

		Scene GetScene_ShadowDebug_10();

		Scene GetScene_ShadowBase_11();

		Scene GetScene_Cloud_11_01();

		Scene GetScene_FoveatedRender_12();

	private:
		TestScene() {}
	};

	class TestPass
	{
	public:
		static TestPass& Intance()
		{
			static TestPass* intance_ = new TestPass();
			return *intance_;
		}

		Pass GetPass_FrameTest_04();

		Pass GetPass2_FrameTest_04(Pass& p);

		Pass GetPass_SkyBox_05();

		Pass GetPass_UniformBufferTest_06();

		Pass GetPass_GeometryNormal_07();

		Pass GetPass_InstancedTest_08();

		Pass GetPass_ShadowMap_09();

		Pass GetPass_BackFaceDepth_09_01();

		Pass GetPass_FrontFaceDepth_09_02();

		Pass GetPass_ShadowDebug_10(Pass &p);

		Pass GetPass_BaseShadow_11(Pass& p);

		Pass GetPass_Cloud_11_01(Pass& shadow, Pass& depth);

		Pass GetPass_FoveatedRendering_12(Pass& p);

		Pass GetPass_FoveatedRendering_Pass2_13(Pass& p);

		Pass GetPass_RayMarchingRendering(Pass& main_tex, Pass& backDepth, Pass& frontDepth);

		void Draw_FrameTest_04(Pass& p1, Pass& p2);

		void DrawShadowTest_05(Pass& p1, Pass& p2);

		void DrawBaseShadow_06(Pass& p1, Pass& p2);

		void DrawFoveated_07(Pass& p1, Pass& p2, Pass& p3);

		void DrawFoveated_Comp_08(Pass& p1, Pass& p2, Pass& p3, Pass& p4);

		void DrawFoveated_Comp_09(Pass& p0, Pass& p1, Pass& p2, Pass& p3, Pass& p4);

	private:
		TestPass() {}
	};

	// The class is used to create post-process pass object.
	// Use Singleton Pattern, so can use anywhere, 
	// and you can not create any object of this class outside it 
	// ...
	class PostPassFactory
	{
	public:

		using MaterialPtr = std::shared_ptr<Material>;
		using PassPtr = std::shared_ptr<Pass>;

		// Return this only instance of the class.
		// ...
		static PostPassFactory& Instance()
		{
			static PostPassFactory* instance_ = new PostPassFactory();
			return *instance_;
		}

		// Create a post-process obj by transporting a post-material.
		// But this is a special case, because here we do not think about 
		// pre-frame, so we usually use it for ray marching.
		// ...
		PassPtr CreateOne(MaterialPtr mat);

		// Create a post-process obj by transporting a post-material.
		// This is the most used, normal case, e.g. bloom, blur etc.
		// ...
		PassPtr CreateOne(PassPtr pre_frame, MaterialPtr mat);


	private:
		PostPassFactory() {}
	};
}


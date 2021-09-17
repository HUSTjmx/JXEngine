#pragma once
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

		Scene GetScene_ShadowDebug_10();

		Scene GetScene_ShadowBase_11();

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

		Pass GetPass_ShadowDebug_10(Pass &p);

		Pass GetPass_BaseShadow_11(Pass& p);

		Pass GetPass_FoveatedRendering_12(Pass& p);

		Pass GetPass_FoveatedRendering_Pass2_13(Pass& p);

		void Draw_FrameTest_04(Pass& p1, Pass& p2);

		void DrawShadowTest_05(Pass& p1, Pass& p2);

		void DrawBaseShadow_06(Pass& p1, Pass& p2);

		void DrawFoveated_07(Pass& p1, Pass& p2, Pass& p3);

		void DrawFoveated_Comp_08(Pass& p1, Pass& p2, Pass& p3, Pass& p4);
	

	private:
		TestPass() {}
	};
}


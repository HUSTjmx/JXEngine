#pragma once

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

		void Draw_FrameTest_04(Pass& p1, Pass& p2);
	

	private:
		TestPass() {}
	};
}


#pragma once

class Scene;

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

	private:
		TestScene() {}
	};
}


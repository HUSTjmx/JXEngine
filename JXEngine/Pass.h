#pragma once
#include <memory>

class FrameBuffer;
class Scene;
class Material;

class Pass
{
public:

	using Input = std::shared_ptr<Scene>;
	using Output = std::shared_ptr<FrameBuffer>;
	using GlobalMat = std::shared_ptr<Material>;

	Pass() {}

	Pass(Input scene) : scene(scene) {}

	Pass(Input s, Output f) : scene(s), frame(f) {}

	Pass(Input s, Output f, GlobalMat sh) : scene(s), frame(f), mat(sh) {}

	virtual void Active();

	virtual void Draw();

	virtual void BindOutput();

	virtual void UpdateInput(Input scene);

	virtual Output GetOutput();

	virtual void UpdateOutput(Output frame);

	virtual GlobalMat GetMat();

	virtual void UpdateGlobalMat(GlobalMat mat);

	Input scene;

private:

	Output frame;

	GlobalMat mat;
};


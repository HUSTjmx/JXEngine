#pragma once
#include <memory>

class FrameBuffer;
class Scene;

class Pass
{
public:
	using Input = std::shared_ptr<Scene>;
	using Output = std::shared_ptr<FrameBuffer>;

	Pass(Input scene) : scene(scene) {}

	virtual void Active();


private:

	Output frame;

	Input scene;
};


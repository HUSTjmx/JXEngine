#include "Timer.h"

void Timer::Reset()
{
	lastFrame = currentFrame;
	deltaTime = 0.0;
}

float Timer::Timing()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	return deltaTime;
}

float Timer::GetCurrentTime() const
{

	return glfwGetTime();
}

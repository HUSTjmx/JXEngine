#include <iostream>
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

float Timer::FPS() const
{
	float fps = 1.0 / deltaTime;
	std::cout << "The fps is : " << (int)fps << std::endl;
	return fps;
}

#include <iostream>
#include "Timer.h"

std::string TimeOutputTrait<TimeOutputType::Second>::_str_1 = " cost time ";
std::string TimeOutputTrait<TimeOutputType::Second>::unit = " s ";

std::string TimeOutputTrait<TimeOutputType::FrameRate>::_str_1 = " FPS ";
std::string TimeOutputTrait<TimeOutputType::FrameRate>::unit = " Frame ";

void Timer::Reset()
{
	lastFrame = currentFrame;
	deltaTime = 0.0;
}

float Timer::Timing(bool isRealTimeShow)
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	if(isRealTimeShow)
		std::cout << "The cost time is : " << deltaTime << std::endl;
	return deltaTime;
}

float Timer::GetCurrentTime() const
{

	return glfwGetTime();
}

float Timer::FPS(bool isRealTimeShow)
{
	float fps = 1.0 / deltaTime;
	if(isRealTimeShow)
		std::cout << "The fps is : " << (int)fps << std::endl;
	return fps;
}

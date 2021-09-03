#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define Clock Timer::Instance()


class Timer
{
public:

	static Timer& Instance()
	{
		static Timer* instance = new Timer();
		return *instance;
	}

	void Reset();

	//��ʱ��
	float Timing();

	float DeltaTime() const {
		return deltaTime;
	}

private:

	Timer() : deltaTime(0.0), lastFrame(0.0), currentFrame(0.0) {}

	float deltaTime;
	
	float lastFrame;

	float currentFrame;
};


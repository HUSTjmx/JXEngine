#pragma once
#include <iostream>
#include <numeric>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "DesignPatterns.h"
#include "Config.h"

#define Clock Timer::Instance()

enum class TimeOutputType
{
	Second,
	FrameRate
};

template<TimeOutputType type_>
class TimeOutputTrait
{
public:
	static std::string _str_1;
	static std::string unit;
};

template<>
class TimeOutputTrait<TimeOutputType::Second>
{
public:
	static std::string _str_1;
	static std::string unit;
};

template<>
class TimeOutputTrait<TimeOutputType::FrameRate>
{
public:
	static std::string _str_1;
	static std::string unit;
};


class Timer
{
public:

	static Timer& Instance()
	{
		static Timer* instance = new Timer();
		return *instance;
	}

	void Reset();

	// Get the cost time.
	// ...
	float Timing(bool isRealTimeShow = false);

	// Get the avarage of cost time.
	// ...
	float AverageCostTime() = delete;

	float DeltaTime() const {
		return deltaTime;
	}

	// Get current time.
	// ...
	float GetCurrentTime()const;

	// Get the FPS.
	// ...
	float FPS(bool isRealTimeShow = false);

	// Get the average of FPS
	// ...
	float AverageFPS() = delete;

	// ...
	void Excute(TimeOutputType type_, bool isRealTimeShow = false) = delete;


private:


	Timer() : deltaTime(0.0), lastFrame(0.0), currentFrame(0.0) {}

	float deltaTime;
	
	float lastFrame;

	float currentFrame;
};


template<int ID, int range = (int)CONFIG::TIME_CONFIG::AverageRange>
class AverageTimeTool : Singleton<AverageTimeTool<ID, range>>
{
public:

	friend class Singleton<AverageTimeTool<ID, range>>;
	std::vector<float> elems;
	std::vector<float> elems_fps;

	float Excute(float elem, float elem_fps, TimeOutputType type_);

private:

	float startFrame;

	float currentFrame;
};

template<int ID, int range>
float AverageTimeTool<ID, range>::Excute(float elem, float elem_fps, TimeOutputType type_)
{
	this->currentFrame = glfwGetTime();
	this->elems.push_back(elem);
	this->elems_fps.push_back(elem_fps);
	float sum;
	std::string name;
	std::string unit;
	if (this->currentFrame - this->startFrame > (float)range)
	{
		if (type_ = TimeOutputType::Second)
		{
			std::accumulate(elems.begin(), elems.end(), sum);
			sum /= elems.size();
			name = TimeOutputTrait<TimeOutputType::Second>::_str_1;
			unit = TimeOutputTrait<TimeOutputType::Second>::unit;
		}
		else
		{
			std::accumulate(elems_fps.begin(), elems_fps.end(), sum);
			sum /= elems_fps.size();
			name = TimeOutputTrait<TimeOutputType::FrameRate>::_str_1;
			unit = TimeOutputTrait<TimeOutputType::FrameRate>::unit;
		}
			
		std::cout << "The average of" << name << "is : " << sum << unit << std::endl;
		this->startFrame = this->currentFrame;
	}
}


#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "Counted.h"

class ShaderCompiler;

enum class PhotometricUnit {
	Power,				//Phi, Lumen(lm)流明
	Intensity,			//I, Candela(cd)或 lm/sr 
	Illuminance,		//E, Lux(lx)或 lm/(m^2)
	Luminance,			//L, Nit(nt) 或 cd/(m^2)
	RadiantPower,		//Phi_e, Watt(W) 瓦特
	LuminousEfficacy,	//rho, Lumens per watt(lm/W)
	LuminousEffciency	//V, %
};

enum class LightType
{
	DirectionLight,
	PointLight,
	SpotLight,
	None
};

class Light
{

public:

	Light(float v) : value(v), lightInRadius(10.0f), color(1.0, 1.0, 1.0), unit(PhotometricUnit::Power) {}
	
	Light(float v, PhotometricUnit m) : value(v), lightInRadius(10.0f), color(1.0, 1.0, 1.0), unit(m) {}

	Light(float v, glm::vec3 c, PhotometricUnit m) : value(v), lightInRadius(10.0f), color(c), unit(m) {}

	virtual void LoadInfoToShader(std::shared_ptr<ShaderCompiler> shader) {}
	
	virtual void SetRadius(float r) { lightInRadius = r; }

	void SetValue(float v);

	void SetColor(glm::vec3 c);

	PhotometricUnit unit;

	glm::vec3 color;

	float value;

	float lightInRadius;
};


class DirectionalLight : public Light, private Counted<DirectionalLight>
{

public:

	DirectionalLight() : Light(105000.0f, PhotometricUnit::Illuminance), direction(glm::vec3(-1.0)){}

	DirectionalLight(glm::vec3 v) : Light(105000.0f, PhotometricUnit::Illuminance), direction(v) {}

	virtual void LoadInfoToShader(std::shared_ptr<ShaderCompiler> shader) override;

	virtual void SetRadius(float r) override {}

	void SetDirection(glm::vec3 d);

	using Counted<DirectionalLight>::ObjectCount;
	using Counted<DirectionalLight>::TooManyObjects;
	using Counted<DirectionalLight>::Index;

private:

	glm::vec3 direction;
};


class PointLight : public Light, private Counted<PointLight>
{
public:

	PointLight() : Light(15.0f), position(glm::vec3(1.0, 2.0, 3.0)) {}

	PointLight(glm::vec3 p) : Light(15.0f), position(p) {}

	virtual void LoadInfoToShader(std::shared_ptr<ShaderCompiler> shader) override;

	using Counted<PointLight>::ObjectCount;
	using Counted<PointLight>::TooManyObjects;

private:

	glm::vec3 position;
};

class SpotLight : public Light, private Counted<SpotLight>
{

public:
	
	SpotLight(glm::vec3 p, glm::vec3 d, float i_a, float o_a, float value) : Light(value), position(p), direction(d), innerAngle(i_a), outerAngle(o_a) {}

	virtual void LoadInfoToShader(std::shared_ptr<ShaderCompiler> shader) override;
	
	using Counted<SpotLight>::ObjectCount;
	using Counted<SpotLight>::TooManyObjects;

private:

	glm::vec3 position;

	glm::vec3 direction;

	float innerAngle;

	float outerAngle;

};


static float Watt2Lumen(float watt, float V)
{
	V = V > 1.0f ? V / 100.0f : V;
	return watt * 683.0f * V;
}
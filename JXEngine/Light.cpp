#include <iostream>
#include "Light.h"
#include "ShaderCompiler.h"
#include "Config.h"

void DirectionalLight::LoadInfoToShader(std::shared_ptr<ShaderCompiler> shader)
{
	std::string preName = CONFIG::LIGHT_SETTINGS::SHADER_DIR_LIGHT::MAIN_NAME + "[" + std::to_string(Index()) + "].";

	shader->SetVec3(preName + CONFIG::LIGHT_SETTINGS::SHADER_DIR_LIGHT::COLOR, color);
	shader->SetVec3(preName + CONFIG::LIGHT_SETTINGS::SHADER_DIR_LIGHT::DIRECTION, direction);
	shader->SetFloat(preName + CONFIG::LIGHT_SETTINGS::SHADER_DIR_LIGHT::INTENSITY, value);

}

void DirectionalLight::SetDirection(glm::vec3 d)
{
	direction = d;
}

void Light::SetValue(float v)
{
	value = v;
}

void Light::SetColor(glm::vec3 c)
{
	color = c;
}

const size_t Counted<DirectionalLight>::maxObjects = CONFIG::LIGHT_SETTINGS::MAX_DIRECTION_LIGHT_NUM;
const size_t Counted<PointLight>::maxObjects = CONFIG::LIGHT_SETTINGS::MAX_POINT_LIGHT_NUM;
const size_t Counted<SpotLight>::maxObjects = CONFIG::LIGHT_SETTINGS::MAX_SPOT_LIGHT_NUM;

void PointLight::LoadInfoToShader(std::shared_ptr<ShaderCompiler> shader)
{
	std::string preName = CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::MAIN_NAME + "[" + std::to_string(Index()) + "].";
	shader->SetVec3(preName + CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::COLOR, color);
	shader->SetVec3(preName + CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::POSITION, position);
	shader->SetFloat(preName + CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::INTENSITY, value);
	shader->SetFloat(preName + CONFIG::LIGHT_SETTINGS::SHADER_POINT_LIGHT::LIGHT_IN_RADIUS, 1.0 / lightInRadius);
}

void SpotLight::LoadInfoToShader(std::shared_ptr<ShaderCompiler> shader)
{
	std::string preName = CONFIG::LIGHT_SETTINGS::SHADER_SPOT_LIGHT::MAIN_NAME + "[" + std::to_string(Index()) + "].";

	shader->SetVec3(preName + CONFIG::LIGHT_SETTINGS::SHADER_SPOT_LIGHT::COLOR, color);
	shader->SetVec3(preName + CONFIG::LIGHT_SETTINGS::SHADER_SPOT_LIGHT::DIRECTION, direction);
	shader->SetVec3(preName + CONFIG::LIGHT_SETTINGS::SHADER_SPOT_LIGHT::POSITION, position);
	shader->SetFloat(preName + CONFIG::LIGHT_SETTINGS::SHADER_SPOT_LIGHT::INNER_ANGLE, innerAngle);
	shader->SetFloat(preName + CONFIG::LIGHT_SETTINGS::SHADER_SPOT_LIGHT::OUTER_ANGLE, outerAngle);
	shader->SetFloat(preName + CONFIG::LIGHT_SETTINGS::SHADER_SPOT_LIGHT::INTENSITY, value);
	shader->SetFloat(preName + CONFIG::LIGHT_SETTINGS::SHADER_SPOT_LIGHT::LIGHT_IN_RADIUS, 1.0 / lightInRadius);

}

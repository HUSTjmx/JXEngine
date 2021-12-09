// Copyright 2021.	All rights reserved
//
// Author: 2568094892@qq.com (Jiang Meng Xian)
//
// this files include atmosphere class.
// ...
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Actor.h"

class Material;

// The atmosphere object, which we can add one to the scene,
// and we can set up it.
// ...
class Atmosphere : public Actor
{
public:

	using Actor::materials;

	Atmosphere(
		glm::vec3 sd = glm::vec3(0, 1, 0),
		float er = 6360e3, float ar = 6420e3,
		float hr = 7994, float hm = 1200) : Actor(),
		sunDirection(sd),
		earthRadius(er),
		atmosphereRadius(ar),
		Hr(hr),
		Hm(hm)
	{
		InitVertex();
	}

	Atmosphere(
		std::shared_ptr<Material> m,
		glm::vec3 sd = glm::vec3(0, 1, 0),
		float er = 6360e3, float ar = 6420e3,
		float hr = 7994, float hm = 1200) : Actor(m),
		sunDirection(sd),
		earthRadius(er),
		atmosphereRadius(ar),
		Hr(hr),
		Hm(hm)
	{
		InitVertex();
	}

	// Load Sky rendering info.
	// ...
	virtual void LoadInfoToShader()override;

	// The sun direction(normalized)
	// ...
	glm::vec3 sunDirection;

	// This is usually Rg or Re (radius ground, eart)
	// ...
	float earthRadius;

	// This is usually R or Ra (radius atmosphere)
	// ...
	float atmosphereRadius;

	// Thickness of the atmosphere if density was uniform (Hr)
	// ...
	float Hr;

	// Same as above but for Mie scattering (Hm)
	// ...
	float Hm;

	// ...
	static const glm::vec3 betaR;

	// ...
	static const glm::vec3 betaM;

private:

	// Init vertex of sky.
	// ..
	void InitVertex();
};



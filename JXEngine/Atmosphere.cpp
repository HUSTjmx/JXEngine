#include "Atmosphere.h"
#include "Config.h"
#include "Tools.h"
#include "VertexModel.h"
#include "EmbeddedData.h"
#include "ShaderCompiler.h"
#include "Material.h"

using VertexModelPtr = std::shared_ptr<VertexModel>;

const glm::vec3 Atmosphere::betaR(3.8e-6f, 13.5e-6f, 33.1e-6f);
const glm::vec3 Atmosphere::betaM(21e-6f);

void Atmosphere::LoadInfoToShader()
{
	for (auto m = materials.begin(); m != materials.end(); ++m)
	{
		m->get()->Active();
		//std::cout << earthRadius << std::endl;
		m->get()->GetShader()->SetFloat(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::EARTH_RADIUS, earthRadius);
		m->get()->GetShader()->SetFloat(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::ATMOSHERE_RADIUS, atmosphereRadius);
		m->get()->GetShader()->SetFloat(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::HR, Hr);
		m->get()->GetShader()->SetFloat(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::HM, Hm);
		m->get()->GetShader()->SetVec3(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::BETA_R, betaR);
		m->get()->GetShader()->SetVec3(CONFIG::MATERIAL_SETTINGS::SKY_MODEL::BETA_M, betaM);
	}
}

void Atmosphere::InitVertex()
{
	std::vector<float> v_d;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> offsets;
	SphereCreator::Instance().Create_ICO(v_d, indices, offsets, 3);
	VertexModelPtr skySphere = std::make_shared<VertexModel>();
	skySphere->BindVertexToBuffer(v_d, indices, offsets);
	this->AddMesh(skySphere);
}

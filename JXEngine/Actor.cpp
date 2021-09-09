#include <iostream>
#include "Actor.h"
#include "Config.h"
#include "Camera.h"
#include "Material.h"
#include "VertexModel.h"
#include "ShaderCompiler.h"
#include "Texture.h"

Actor::Actor(std::shared_ptr<Material> m)
{
	SetMaterial(m);
	InitPosition();
}

Actor::Actor(const std::string& path, std::shared_ptr<Material> m)
{
	SetMaterial(m);
	loadModel(path);
	InitPosition();
}

Actor::Actor(std::shared_ptr<VertexModel> m, std::shared_ptr<Material> n)
{
	AddMesh(m);
	SetMaterial(n);
	InitPosition();
}

Actor& Actor::operator=(const Actor& a)
{
	meshes = a.meshes;
	SetMaterial(a.material);
	transform = a.transform;
	positions = a.positions;
	return *this;
}

void Actor::AddMesh(std::shared_ptr<VertexModel> m)
{
	meshes.push_back(m);
}

void Actor::SetMaterial(std::shared_ptr<Material> m)
{
	material = m;
	type.addObserver(material.get());
	//type.notify(&type);
}

void Actor::SetPostionsArray(std::vector<glm::vec3>& p)
{
	positions = p;
}

std::shared_ptr<Material> Actor::GetMaterial()
{
	return material;
}

void Actor::Draw()
{
	material->Active();
	material->engineSetting->InitExecutive();
	material->BindTexturesToOpenGL();
	for (auto mesh = meshes.begin(); mesh != meshes.end();++mesh)
	{
		mesh->get()->BindVAO();
		for (int i = 0; i < positions.size();i++)
		{
			transform.Reset();
			AnimaTo(positions[i], transform.scale, 0.0, transform.rotation_axis);
			material->SetP(transform);
			mesh->get()->Draw();
		}
	}
	material->engineSetting->EndExecutive();
}

void Actor::Draw(std::shared_ptr<Material> newMat)
{
	material->Active();
	newMat->BindTexturesToOpenGL();
	for (auto mesh = meshes.begin(); mesh != meshes.end();++mesh)
	{
		mesh->get()->BindVAO();
		for (int i = 0; i < positions.size();i++)
		{
			transform.Reset();
			AnimaTo(positions[i], transform.scale, 0.0, transform.rotation_axis);
			newMat->SetP(transform);
			mesh->get()->Draw();
		}
	}
}

void Actor::Delete()
{
	for (auto mesh = meshes.begin(); mesh != meshes.end();++mesh)
	{
		mesh->get()->Delete();
	}
}

void Actor::Anima(glm::vec3 pos_delta, glm::vec3 sca_delta, float angle, glm::vec3 axis)
{
	transform.Move(pos_delta);
	transform.Scale(sca_delta);
	transform.Rotate(axis, angle);
}

void Actor::AnimaTo(glm::vec3 pos, glm::vec3 scale, float angle, glm::vec3 axis)
{
	transform.MoveTo(pos);
	transform.ScaleTo(scale);
	transform.Rotate(axis, angle);
}

void Actor::ScaleTo(glm::vec3 scale)
{
	transform.ScaleTo(scale);
}

void Actor::loadModel(std::string const& path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

void Actor::processNode(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

std::shared_ptr<VertexModel> Actor::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	this->material->LoadTextures(material, directory);

	// return a mesh object created from the extracted mesh data
	std::shared_ptr<VertexModel> v = std::make_shared<VertexModel>();
	v->BindVertexToBuffer(vertices, indices);
	
	return v;
}

void Actor::InitPosition()
{
	positions.push_back(glm::vec3(0.0));
}



//void Actor::SetP(int i)
//{
//	transform.Reset();
//	AnimaTo(positions[i], glm::vec3(1.0, 1.0, 1.0), 30.0f * i);
//	material->GetShader()->SetMat4(CONFIG::SHADER_DEFAULT_UNIFORM_NAME::MODEL_MATRIX, transform.model);
//}

void Transform::Move(glm::vec3 delta)
{
	position += delta;
	model = glm::translate(model, position);
}

void Transform::MoveTo(glm::vec3 newPosition)
{
	position = newPosition;
	model = glm::translate(model, position);
}

void Transform::Scale(glm::vec3 delta)
{
	scale += delta;
	model = glm::scale(model, scale);
}

void Transform::ScaleTo(glm::vec3 newScale)
{
	scale = newScale;
	model = glm::scale(model, scale);
}

void Transform::Rotate(glm::vec3 Axis, float angle)
{
	rotation_axis = Axis;
	this->angle = angle;
	model = glm::rotate(model, glm::radians(angle), Axis);
}

void Transform::Reset()
{
	model = glm::mat4(1.0f);
}

void ActorType::SetMobility(Mobility a)
{
	mobility = a;
	this->notify(this);
}

ActorType::Mobility ActorType::GetMobility()
{
	return mobility;
}

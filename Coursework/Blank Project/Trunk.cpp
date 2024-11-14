#include "Trunk.h"

Trunk::Trunk(float hSize, GLuint* icicleMask, GLuint* iceTexture, GLuint* woodTexture, GLuint* woodNormal, GLuint* iceBumpTex, Matrix4* offsetBatch)
{	

	//SetFromSubMesh("Trunk");
	mesh = Mesh::LoadFromMeshFile("Trunk.msh");
	SetMesh(mesh);

	GetMaterial()->AddProperty("batch", offsetBatch);

	GetMaterial()->AddProperty("diffuse", woodTexture);
	GetMaterial()->AddProperty("woodNormal", woodNormal);
	GetMaterial()->AddProperty("icicleMask", icicleMask);
	GetMaterial()->AddProperty("iceTex", iceTexture);
	GetMaterial()->AddProperty("iceBumpTex", iceBumpTex);


	SetColour(Vector4(0, 0, 0, 1));
	SetModelScale(Vector3(25, 25, 25));

	GetMaterial()->AddProperty("temperature", Material::Temperature);
	GetMaterial()->AddProperty("iceHeight", 25.0f);

	GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);

	//GetMaterial()->AddProperty("tessBuffer", Material::TessTri);
	//GetMaterial()->AddProperty("tessLevelInner", 10.0f);
	//GetMaterial()->AddProperty("tessLevelOuter", 10.0f);	

}



void Trunk::Update(float dt, Vector3 cameraPos)
{
	SceneNode::Update(dt, cameraPos);
}

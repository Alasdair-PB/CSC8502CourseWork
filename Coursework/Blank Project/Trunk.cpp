#include "Trunk.h"

Trunk::Trunk(float hSize, GLuint icicleMask, GLuint iceTexture)
{	
	//GetMaterial()->AddProperty("icicleMask", icicleMask);
	GetMaterial()->AddProperty("iceTex", iceTexture);
	SetFromSubMesh("Trunk");

	SetColour(Vector4(0, 0, 0, 1));
	SetTransform(Matrix4::Translation(Vector3(hSize * 0.5f, 165, hSize * 0.5f)));
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

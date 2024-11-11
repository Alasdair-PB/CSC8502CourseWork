#include "Trunk.h"

Trunk::Trunk(float hSize, GLuint icicleMask)
{
	GetMaterial()->AddProperty("icicleMask", icicleMask);
	SetFromSubMesh("Trunk");
	GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);

	SetColour(Vector4(0, 0, 0, 1));
	SetTransform(Matrix4::Translation(Vector3(hSize * 0.5f, 165, hSize * 0.5f)));
	SetModelScale(Vector3(25, 25, 25));

	GetMaterial()->AddProperty("temperature", Material::Temperature);
	GetMaterial()->AddProperty("iceHeight", 50.0f);
	GetMaterial()->AddProperty("tessBuffer", Material::TessTri);
	//GetMaterial()->AddProperty("tessLevelInner", 10.0f);
	//GetMaterial()->AddProperty("tessLevelOuter", 10.0f);	
	GetMaterial()->AddProperty("bumpTex", icicleMask);
}


void Trunk::Update(float dt) {
	SceneNode::Update(dt);
}
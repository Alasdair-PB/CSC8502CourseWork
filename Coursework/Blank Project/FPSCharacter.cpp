#include "FPSCharacter.h"

FpsCharacter::FpsCharacter(float hSize)
{
	anim = new MeshAnimation("FPSCharacter.anm");		
	GetMaterial()->AddProperty("animation", anim);
	SetFromSubMesh("FPSCharacter");	

	GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);

	SetColour(Vector4(0, 0, 0, 1));
	//SetTransform(Matrix4::Translation(Vector3((hSize * 0.5f) + 100, 165, hSize * 0.5f)));
	SetModelScale(Vector3(100, 100, 100));
}


void FpsCharacter::Update(float dt, Vector3 cameraPos)
{
	SceneNode::Update(dt, cameraPos);
}
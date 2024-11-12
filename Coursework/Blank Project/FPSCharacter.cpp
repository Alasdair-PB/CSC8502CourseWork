#include "FPSCharacter.h"

FpsCharacter::FpsCharacter(float hSize)
{
	anim = new MeshAnimation("FPSCharacter.anm");	
	SetFromSubMesh("FPSCharacter");	
	GetMaterial()->AddProperty("animation", anim);

	GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);

	SetColour(Vector4(0, 0, 0, 1));
	SetTransform(Matrix4::Translation(Vector3((hSize * 0.5f) + 100, 165, hSize * 0.5f)));
	SetModelScale(Vector3(100, 100, 100));
}


void FpsCharacter::Update(float dt) {
	SceneNode::Update(dt);
}
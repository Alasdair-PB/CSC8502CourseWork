#include "Rock.h"


Rock::Rock(Matrix4* offsets)
{

	SetFromSubMesh("RockAfbx");
	GetMaterial()->AddProperty("batch", offsets);

	GetMaterial()->AddProperty("cameraPosition", Material::CameraPosition);
	GetMaterial()->AddProperty("faceCullOff", Material::DualFace);
	GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);

	SetColour(Vector4(0, 0, 0, 1));
	SetModelScale(Vector3(25, 25, 25));
}



void Rock::Update(float dt, Vector3 cameraPos)
{
	SceneNode::Update(dt, cameraPos);
}

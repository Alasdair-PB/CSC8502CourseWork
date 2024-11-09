#include "Trunk.h"

Trunk::Trunk(float hSize)
{
	SetFromSubMesh("Trunk");
	GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);
	GetMaterial()->AddProperty("cameraPosition", Material::CameraPosition);

	SetColour(Vector4(0, 0, 0, 1));
	SetTransform(Matrix4::Translation(Vector3(hSize * 0.5f, 165, hSize * 0.5f)));
	SetModelScale(Vector3(25, 25, 25));
}


void Trunk::Update(float dt) {
	SceneNode::Update(dt);
}
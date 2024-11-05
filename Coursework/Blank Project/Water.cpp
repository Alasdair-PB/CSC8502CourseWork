#include "Water.h"

Water::Water(GLuint texture, float hSize)
{
	quad = Mesh::GenerateQuad();
	SetMesh(quad);
	SetModelScale(Vector3(hSize * 0.5f, hSize * 0.5f, hSize * 0.5f));

	SetColour(Vector4(0, 0, 0, 1));

	GetMaterial()->AddProperty("diffuseTex", texture);
	GetMaterial()->AddProperty("cameraPos", Material::CameraPosition);
	GetMaterial()->AddProperty("cubeMap", Material::CubeMap);

	SetTransform(Matrix4::Translation(Vector3(hSize * 0.5f,0, hSize * 0.5f)) * Matrix4::Rotation(90.0f, Vector3(1, 0, 0)));
	SetBoundingRadius(15.0f);
}


void Water::Update(float dt) {
	SceneNode::Update(dt);
}
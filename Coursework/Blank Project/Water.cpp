#include "Water.h"

Water::Water(GLuint texture)
{
	quad = Mesh::GenerateQuad();
	SetMesh(quad);
	SetColour(Vector4(0, 0, 0, 0.3f));
	SetModelScale(Vector3(10, 15, 5));
	SetTexture(texture);
	SetTransform(Matrix4::Translation(Vector3(0, 0, 0)) * Matrix4::Rotation(90.0f, Vector3(1, 0, 0)));
	SetBoundingRadius(15.0f);
}


void Water::Update(float dt) {
	SceneNode::Update(dt);
}
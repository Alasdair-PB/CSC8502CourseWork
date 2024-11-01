#include "Water.h"

Water::Water()
{
	quad = Mesh::GenerateQuad();
	texture = SOIL_load_OGL_texture(TEXTUREDIR "water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	water = new SceneNode(quad, Vector4(0, 0, 0, 0.3f));
	water ->SetModelScale(Vector3(10, 15, 5));
	water->SetTexture(texture);
	water->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)) * Matrix4::Rotation(90.0f, Vector3(1, 0, 0)));
	AddChild(water);

	water->SetBoundingRadius(15.0f);
}


void Water::Update(float dt) {
	SceneNode::Update(dt);
}
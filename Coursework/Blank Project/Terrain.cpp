#include "Terrain.h"
#include "../nclgl/HeightMap.h"

Terrain::Terrain(GLuint texture)
{
	ground = new SceneNode(new HeightMap(TEXTUREDIR "noise.png"), Vector4(0, 0, 0, 1));
	ground->SetModelScale(Vector3(10, 15, 5));
	ground->SetTransform(Matrix4::Translation(Vector3(0, -1.0f, 0)) * Matrix4::Rotation(180.0f, Vector3(1, 0, 0)));
	ground->SetTexture(texture);

	AddChild(ground);
	ground->SetBoundingRadius(15.0f);

}


void Terrain::Update(float dt) {
	SceneNode::Update(dt);
}
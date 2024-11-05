#include "Terrain.h"
#include "../nclgl/HeightMap.h"

Terrain::Terrain(GLuint texture, GLuint textureBump)
{
	HeightMap* heightMap = new HeightMap(TEXTUREDIR "noise.png");
	mapSize = heightMap->GetHeightmapSize();
	SetMesh(heightMap);
	SetColour(Vector4(0, 0, 0, 1));

	GetMaterial()->AddProperty("diffuseTex", texture);
	GetMaterial()->AddProperty("bumpTex", textureBump);
	GetMaterial()->AddProperty("lighting", Material::LightRender);


	//SetModelScale(Vector3(10, 15, 5));
	//SetTransform(Matrix4::Translation(Vector3(0, -1.0f, 0)) * Matrix4::Rotation(180.0f, Vector3(1, 0, 0)));
	SetBoundingRadius(15.0f);
}

void Terrain::Update(float dt) {
	SceneNode::Update(dt);
}

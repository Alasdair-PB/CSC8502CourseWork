#include "Terrain.h"
#include "../nclgl/HeightMap.h"

Terrain::Terrain(GLuint texture, GLuint textureBump)
{
	HeightMap* heightMap = new HeightMap(TEXTUREDIR "noise.png");
	mapSize = heightMap->GetHeightmapSize();

	SetMesh(heightMap);
	SetColour(Vector4(0, 0, 0, 1)); // Not transparent

	GetMaterial()->AddProperty("diffuseTex", texture);
	GetMaterial()->AddProperty("bumpTex", textureBump);
	SetBoundingRadius(15.0f);
}

void Terrain::Update(float dt) {
	SceneNode::Update(dt);
}

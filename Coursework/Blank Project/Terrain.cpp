#include "Terrain.h"
#include "../nclgl/HeightMap.h"

Terrain::Terrain(GLuint texture, GLuint textureBump)
{
	HeightMap* heightMap = new HeightMap(TEXTUREDIR "noise.png");
	mapSize = heightMap->GetHeightmapSize();

	SetMesh(heightMap);
	SetColour(Vector4(0, 0, 0, 1.1f));

	GetMaterial()->AddProperty("diffuseTex", texture);
	GetMaterial()->AddProperty("bumpTex", textureBump);
	GetMaterial()->AddProperty("lighting", Material::LightRender);
	GetMaterial()->AddProperty("cameraPos", Material::CameraPosition);


	SetBoundingRadius(15.0f);
}

void Terrain::Update(float dt) {
	SceneNode::Update(dt);
}

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
	GetMaterial()->AddProperty("lightRender", Material::LightRender);
	GetMaterial()->AddProperty("shadowMapping", Material::ShadowMap);

	SetBoundingRadius(15.0f);
}


void Terrain::Update(float dt, Vector3 cameraPos)
{
	SceneNode::Update(dt, cameraPos);
}

// May use may not- thinking about terrain layers for layered textures, but may be out of scope
// would still need to pass in fixed size arrays for uniform vec2 layerOffsets[8]; so still wouldn't be dynamic
GLuint Terrain::CreateTextureAtlas(const std::vector<GLuint>& textures, int singleTextureWidth, int singleTextureHeight, int atlasCols) {
	int numTextures = textures.size();
	int atlasRows = (numTextures + atlasCols - 1) / atlasCols;
	int atlasWidth = atlasCols * singleTextureWidth;
	int atlasHeight = atlasRows * singleTextureHeight;

	GLuint atlasTexture;
	glGenTextures(1, &atlasTexture);
	glBindTexture(GL_TEXTURE_2D, atlasTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasWidth, atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < numTextures; ++i) {
		int x = (i % atlasCols) * singleTextureWidth;
		int y = (i / atlasCols) * singleTextureHeight;

		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 0, 0, singleTextureWidth, singleTextureHeight);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return atlasTexture;
}

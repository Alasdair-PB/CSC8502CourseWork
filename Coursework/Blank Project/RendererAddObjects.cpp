#include "Renderer.h"
#include "../nclgl/Light.h"
#include "Terrain.h"
#include "Water.h"
#include "Leaves.h"
#include "Trunk.h"
#include "FPSCharacter.h"


bool Renderer::SetTerrain(SceneNode* root)
{
	GLuint* newTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	GLuint* newBumpTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//GLuint* newGrassTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//GLuint* newGrassBumpTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//GLuint* newSnowTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//GLuint* newSnowBumpTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!newTexture || !newBumpTexture)
		return false;

	SetTextureRepeating(*newTexture, true);
	SetTextureRepeating(*newBumpTexture, true);

	Shader* newTerrainShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl");

	texture.push_back(newTexture);
	textureBump.push_back(newBumpTexture);
	shader.emplace_back(newTerrainShader);

	if (!newTerrainShader->LoadSuccess()) 
		return false;

	Terrain* terrain = new Terrain(*newTexture, *newBumpTexture);
	terrain->SetShader(newTerrainShader);

	mapSize = terrain->GetMapSize();
	root->AddChild(terrain);

	return true;
}


bool Renderer::SetFPSCharacter(SceneNode* root)
{
	Shader* newShader = new Shader("SkinningVertex.glsl", "fpsCharFragment.glsl");
	SceneNode* fpsCharacter = new FpsCharacter(mapSize.x);
	fpsCharacter->SetShader(newShader);
	root->AddChild(fpsCharacter);
	return true;
}

bool Renderer::SetTree(SceneNode* root)
{
	Shader* newShader = new Shader("TexturedVertex.glsl", "leafFragment.glsl", "WiggleGeometry.glsl");
	Shader* newTrunkShader = new Shader("icicleVertex.glsl", "icicleFragment.glsl", "", "icicleTessControl.glsl", "icicleTessEvaluation.glsl");
	GLuint* newTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "IceOffset.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	SetTextureRepeating(*newTexture, true);

	texture.push_back(newTexture);
	shader.emplace_back(newTrunkShader);
	shader.emplace_back(newShader);

	SceneNode* leaves = new Leaves(mapSize.x);
	SceneNode* trunk = new Trunk(mapSize.x, *newTexture);

	trunk->SetShader(newTrunkShader);
	leaves->SetShader(newShader);

	root->AddChild(leaves);
	root->AddChild(trunk);
	return true;
}

bool Renderer::SetWater(SceneNode* root)
{
	GLuint* newTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	GLuint* newBumpTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "waterbump.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	Shader* waterShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl", "", "TessellationQuadControl.glsl", "waterTessEvaluation.glsl");
	//Shader* waterShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl", "", "TessellationControl.glsl", "TessellationEvaluation.glsl");

	SetTextureRepeating(*newTexture, true);
	SetTextureRepeating(*newBumpTexture, true);

	if (!waterShader->LoadSuccess())
		return false;

	texture.push_back(newTexture);
	texture.push_back(newBumpTexture);
	shader.emplace_back(waterShader);

	Water* water = new Water(*newTexture, *newBumpTexture, mapSize.x);
	water->SetShader(waterShader);

	root->AddChild(water);
	return true;

}

bool Renderer::SetCubeMap()
{
	skyQuad = Mesh::GenerateQuad();

	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg",
		TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg",
		TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap) {
		return false;
	}

	return true;
}


void Renderer::SetLights()
{
	pointLights = new Light[LIGHT_NUM];
	light = new Light(mapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 0.8f, 0.5f, 1), mapSize.x * 0.5f);

	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
		l.SetPosition(Vector3(rand() % (int)mapSize.x,
			150.0f,
			rand() % (int)mapSize.z));

		l.SetColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));
		l.SetRadius(1500.0f + (rand() % 250));
	}
}

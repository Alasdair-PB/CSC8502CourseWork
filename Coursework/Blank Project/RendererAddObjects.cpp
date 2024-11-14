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

	//Shader* newTerrainShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl");
	Shader* newTerrainShader = new Shader("shadowscenevert.glsl", "shadowscenefrag.glsl");


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
	runningGuy = fpsCharacter;
	Vector3 cameraPos = camera->GetPosition();
	float yaw = camera->GetYaw();
	float pitch = camera->GetPitch();

	cameraPos.y -= 180;

	fpsCharacter->SetTransform(
		Matrix4::Translation(cameraPos) * Matrix4::Rotation(pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(yaw + 180, Vector3(0, 1, 0))
	);

	runningGuy = fpsCharacter;
	root->AddChild(fpsCharacter);
	return true;
}

bool Renderer::SetTree(SceneNode* root)
{
	Shader* newShader = new Shader("TexturedVertex.glsl", "leafFragment.glsl", "WiggleGeometry.glsl");
	Shader* newTrunkShader = new Shader("icicleVertex.glsl", "icicleFragment.glsl", "IcicleGeometry.glsl"); //, "icicleTessControl.glsl", "icicleTessEvaluation.glsl");
	GLuint* newTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "IceOffset.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	GLuint* newIceTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Ice_03_basecolor.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	SetTextureRepeating(*newTexture, true);

	texture.push_back(newTexture);
	shader.emplace_back(newTrunkShader);
	shader.emplace_back(newShader);

	SceneNode* leaves = new Leaves(mapSize.x);
	SceneNode* trunk = new Trunk(mapSize.x, *newTexture, *newIceTexture);

	trunk->SetShader(newTrunkShader);
	leaves->SetShader(newShader);

	root->AddChild(leaves);
	root->AddChild(trunk);
	return true;
}

bool Renderer::SetWater(SceneNode* root)
{
	GLuint* newTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	GLuint* newIceTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Ice_03_basecolor.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	GLuint* newBumpTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "waterbump.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	GLuint* newIceBumpTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Ice_03_normal.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	Shader* waterShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl", "", "TessellationQuadControl.glsl", "waterTessEvaluation.glsl");

	SetTextureRepeating(*newTexture, true);
	SetTextureRepeating(*newBumpTexture, true);
	SetTextureRepeating(*newIceTexture, true);
	SetTextureRepeating(*newIceBumpTexture, true);


	if (!waterShader->LoadSuccess())
		return false;

	texture.push_back(newTexture);
	texture.push_back(newBumpTexture);
	shader.emplace_back(waterShader);

	Water* water = new Water(*newTexture, *newBumpTexture, *newIceTexture, *newIceBumpTexture, mapSize.x);
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

	Vector3 offset = Vector3(mapSize.x * 0.5, 100, mapSize.x * 0.5); //Vector3(0, 0, 0); // 
	light = new Light(Vector3(-20.0f, 10.0f, -20.0f), Vector4(1, 1, 1, 1), 250.0f);
	light->SetPosition(Vector3(-20.0f, 0, -20.0f) + offset);

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

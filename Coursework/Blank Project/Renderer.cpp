#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "Terrain.h"
#include "Water.h"
#include "../nclgl/Light.h"
#include <algorithm>

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	camera = new Camera(0.0f, 0.0f, Vector3(0, 50, 750.0f));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	root = new SceneNode();

	if (!SetCubeMap() || !SetTerrain(root) || !SetWater(root)) //
		return; // Will throw errors as deleting shaders that have not been assigned on ~Renderer

	// Map size setup in SetTerrain
	light = new Light(mapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), mapSize.x);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer(void)	
{
	for (auto tex : texture) {
		glDeleteTextures(1, tex);  
		delete tex;                  
	}
	texture.clear(); 

	for (auto bumpTex : textureBump) {
		glDeleteTextures(1, bumpTex);  
		delete bumpTex;                 
	}
	textureBump.clear(); 

	for (auto shdr : shader) {
		delete shdr; 
	}
	shader.clear();

	delete camera;
	delete skyQuad;

	delete root;
	delete skyboxShader;

	glDeleteTextures(1, &currentBumpMap);
	glDeleteTextures(1, &currentTexture);

}


void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(dt);
}

void Renderer::RenderScene()	
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BuildNodeLists(root);
	SortNodeLists();

	DrawSkybox();
	DrawNodes();
	ClearNodeLists();

}

void Renderer::BuildNodeLists(SceneNode* from) 
{
	Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));

	if (from->GetColour().w < 1.0f) 
		transparentNodeList.push_back(from);
	else 
		nodeList.push_back(from);

	for (auto i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists(*i);
	}
}

void Renderer::DrawNode(SceneNode* n) 
{
	if (n->GetMesh() && n->GetShader()) 
	{
		if (currentShader != n->GetShader()) 
		{
			currentShader = n->GetShader();
			BindShader(currentShader);
			UpdateShaderMatrices();
		}

		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		if (n->reflect) {
			glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
		}

		currentTexture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, currentTexture);

		if (n->reflect) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
			//modelMatrix = Matrix4::Translation(mapSize * 0.5f) * Matrix4::Scale(mapSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));
		}

		// Just for testing
		//modelMatrix = Matrix4::Translation(mapSize * 0.5f) * Matrix4::Scale(mapSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));
		// End of test code
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), currentTexture ? 1 : 0);
		n->Draw(*this);
	}
}


void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::DrawSkybox() {

	glDepthMask(GL_FALSE);
	BindShader(skyboxShader);
	UpdateShaderMatrices();
	skyQuad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}


bool Renderer::SetTerrain(SceneNode* root)
{
	GLuint* newTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	GLuint* newBumpTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

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

	Terrain* terrain = new Terrain(*newTexture);
	terrain->reflect = false;
	terrain->SetShader(newTerrainShader);

	mapSize = terrain->GetMapSize();
	root->AddChild(terrain);

	return true;
}

bool Renderer::SetWater(SceneNode* root)
{
	GLuint* newTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	Shader* waterShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl"); //Shader("reflectVertex.glsl", "reflectFragment.glsl"); /
	SetTextureRepeating(*newTexture, true);

	if (!waterShader->LoadSuccess())
		return false;

	texture.push_back(newTexture);
	shader.emplace_back(waterShader);


	Water* water = new Water(*newTexture);
	water->reflect = true;
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

	if (!cubeMap)
		return false;
	return true;
}
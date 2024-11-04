#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "Terrain.h"
#include "Water.h"
#include "../nclgl/Light.h"

#include <algorithm>


bool Renderer::SetTerrain(SceneNode* root) 
{
	texture = SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	textureBump = SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!texture)
		return false;

	SetTextureRepeating(texture, true);
	SetTextureRepeating(textureBump, true);

	terrainShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl");

	if (!terrainShader->LoadSuccess())
		return false;

	Terrain* terrain = new Terrain(texture);
	mapSize = terrain->GetMapSize();
	terrain->SetShader(terrainShader);
	root->AddChild(terrain);

	return true;
}

bool Renderer::SetWater(SceneNode* root)
{
	waterShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	if (!waterShader->LoadSuccess())
		return false;
	
	Water* water = new Water();
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

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	camera = new Camera(0.0f, 0.0f, Vector3(0, 50, 750.0f));
	light = new Light(mapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), mapSize.x);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	root = new SceneNode();

	if (!SetCubeMap() || !SetTerrain(root) || !SetWater(root))
		return; // Will throw errors as deleting shaders that have not been assigned on ~Renderer

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	init = true;
}

Renderer::~Renderer(void)	
{
	delete waterShader;
	delete terrainShader;
	delete camera;
	delete skyQuad;

	delete root;
	delete skyboxShader;

	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &textureBump);
}


void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(dt);
}

void Renderer::RenderScene()	
{
	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

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

		texture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), texture ? 1 : 0);
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

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}
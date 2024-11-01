#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "Terrain.h"
#include "Water.h"

#include <algorithm>


void Renderer::SetTerrain(SceneNode* root) 
{
	texture = SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	textureBump = SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!texture)
		return;

	SetTextureRepeating(texture, true);
	SetTextureRepeating(textureBump, true);

	terrainShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl");

	if (!terrainShader->LoadSuccess())
		return;

	Terrain* terrain = new Terrain(texture);
	terrain->SetShader(terrainShader);
	root->AddChild(terrain);
}

void Renderer::SetWater(SceneNode* root)
{
	waterShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	if (!waterShader->LoadSuccess())
		return;
	
	Water* water = new Water();
	water->SetShader(waterShader);
	root->AddChild(water);

}

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	camera = new Camera(0.0f, 0.0f, Vector3(0, 50, 750.0f));

	root = new SceneNode();
	SetTerrain(root);
	SetWater(root);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init = true;
}

Renderer::~Renderer(void)	{
	delete waterShader;
	delete terrainShader;
	delete camera;
	delete root;

	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &textureBump);
}


void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(dt);
}

void Renderer::RenderScene()	{
	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(terrainShader);
	BindShader(waterShader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(terrainShader->GetProgram(), "diffuseTex"), 0);

	DrawNodes();

	ClearNodeLists();

}

void Renderer::BuildNodeLists(SceneNode* from) 
{
	Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));

	if (from->GetColour().w < 1.0f) {
		transparentNodeList.push_back(from);
	}
	else {
		nodeList.push_back(from);
	}

	for (auto i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists(*i);
	}
}


void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(n->GetShader()->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(n->GetShader()->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		texture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(glGetUniformLocation(n->GetShader()->GetProgram(), "useTexture"), texture);

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
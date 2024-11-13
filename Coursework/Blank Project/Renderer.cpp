#include "Renderer.h"
#include <algorithm>

#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/Pathing.h"

SceneNode* nodesphere;
SceneNode* nextSphere;

GLuint sphereTexture;
GLuint sphereBumpTexture;

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{	
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	root = new SceneNode();

	if (!SetCubeMap() || !SetTerrain(root) || !SetWater(root) || !SetTree(root))
		return;	

	Vector3 cameraPos = mapSize * Vector3(0.5f, 5.0f, 0.5f);
	//cameraPos.y = 250.0f;
	camera = new Camera(-30.0f, 315.0f, Vector3(-8.0f, 5.0f, 8.0f));

	//camera = new Camera(-45.0f, 0.0f, cameraPos);
	camera->GetPath()->SetPathPattern(cameraPos, Vector3(800,0,0), Vector3(-800,0,0), Vector3(0,0,500), 3);
	lastCameraPos = cameraPos;

	// -------------------------------------------------------------------------------------------------
	shadowScene = new Shader("shadowscenevert.glsl", "shadowscenefrag.glsl");

	sphereTexture = SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	sphereBumpTexture = SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(sphereTexture, true);
	SetTextureRepeating(sphereBumpTexture, true);

	nextSphere = new SceneNode(Mesh::GenerateQuad(), Vector4(1, 1, 1, 1));
	nextSphere->SetTransform(Matrix4::Translation(Vector3(0, -1, 0)) * Matrix4::Rotation(90, Vector3(1, 0, 0)));
	nextSphere->SetShader(shadowScene);
	nextSphere->GetMaterial()->AddProperty("diffuseTex", sphereTexture);
	nextSphere->GetMaterial()->AddProperty("bumpTex", sphereBumpTexture);
	nextSphere->GetMaterial()->AddProperty("lightRender", Material::LightRender);
	nextSphere->GetMaterial()->AddProperty("shadowMapping", Material::ShadowMap);
	nextSphere->GetMaterial()->AddProperty("cameraPos", Material::CameraPosition);

	root->AddChild(nextSphere);

	Vector3 lightPos = mapSize * Vector3(0.5f, 5.0f, 0.5f);

	nodesphere = new SceneNode(Mesh::LoadFromMeshFile("Sphere.msh"), Vector4(1, 1, 1, 1));
	nodesphere->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	nodesphere->SetShader(shadowScene);
	nodesphere->GetMaterial()->AddProperty("diffuseTex", sphereTexture);
	nodesphere->GetMaterial()->AddProperty("bumpTex", sphereBumpTexture);
	nodesphere->GetMaterial()->AddProperty("lightRender", Material::LightRender);
	nodesphere->GetMaterial()->AddProperty("shadowMapping", Material::ShadowMap);
	nodesphere->GetMaterial()->AddProperty("cameraPos", Material::CameraPosition);

	root->AddChild(nodesphere);

	// -------------------------------------------------------------------------------------------------

	SetFPSCharacter(root);
	SetProjectionMatrix();
	this->temperature = -10.0f;
	this->dt = 0;
	this->dtSeason = 0;
	this->currentFrame = 0;
	this->frameTime = 0.0f;

	SetupDepthbuffer();
	SetupFramebuffer();
	SetLights();
	SetupDeferredbuffer();
	SetUpShadowMapBuffer();

	sceneShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl"); 
	pointlightShader = new Shader("pointlightvertex.glsl", "pointlightfrag.glsl");
	combineShader = new Shader("combinevert.glsl", "combinefrag.glsl");
	postProcessShader = new Shader("combinevert.glsl", "postProcessPass.glsl"); 
	fallBackShader = new Shader("shadowVert.glsl", "shadowFrag.glsl");

	if (!sceneShader->LoadSuccess() || !pointlightShader->LoadSuccess() || !combineShader->LoadSuccess()  || !postProcessShader->LoadSuccess()) 
		return;
	
	GLEnablers();
	init = true;
}

void Renderer::SetProjectionMatrix() { projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);}

void Renderer::UpdateScene(float dt) 
{	
	UpdateFrameTime(dt);
	UpdateTemperature(dt);
	camera->UpdateCamera(dt);

	UpdateRunner();
	viewMatrix = camera->BuildViewMatrix();
	SetProjectionMatrix();
	root->Update(dt, camera->GetPosition());
}

void Renderer::UpdateTemperature(float dt) 
{
	this->dt += dt;
	this->dtSeason += dt;
	float nextTemperature = this->temperature;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_1))
		nextTemperature += 0.3f;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_2))
		nextTemperature -= 0.3f;

	if ((nextTemperature < 0 && temperature > 0) || (nextTemperature > 0 && temperature < 0))
		this->dtSeason = 0;
	this->temperature = nextTemperature;
}


void Renderer::UpdateRunner() 
{
	Vector3 cameraPos = camera->GetPosition();
	Vector3 cameraToPlayer = cameraPos;

	cameraToPlayer.y -= 150.0f;

	float yaw = camera->GetYaw();

	float offsetDistance = -25.0f;
	cameraToPlayer.x += offsetDistance * std::sin(yaw * (PI / 180.0f));
	cameraToPlayer.z += offsetDistance * std::cos(yaw * (PI / 180.0f));

	Matrix4 nextPos = Matrix4::Translation(cameraToPlayer) *
		Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(180, Vector3(0, 1, 0));

	Vector3 offsetMatrix = Vector3(0, -99999.0f, 0);

	if (lastCameraPos == cameraPos)
		nextPos = nextPos * Matrix4::Translation(offsetMatrix);

	lastCameraPos = cameraPos;
	runningGuy->SetTransform(nextPos);
}

void Renderer::UpdateFrameTime(float dt)
{
	frameTime -= dt;
}

void Renderer::DepthBufferWrite() 
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	DrawOpaque();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::ShadowBufferWrite() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45);
	shadowMatrix = projMatrix * viewMatrix;

	//DrawOpaque();
	BindShader(fallBackShader);
	DrawDepthNodes(fallBackShader);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
}

void Renderer::DeferredBufferWrite() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	UpdateShaderMatrices();
	DrawNodes();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
	
	SetProjectionMatrix();
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BuildNodeLists(root);
	SortNodeLists();

	DepthBufferWrite();	
	ShadowBufferWrite();
	DrawSkybox();

	DeferredBufferWrite();
	DrawPointLights();
	CombineBuffers();
	PostProcess();

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

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), [](SceneNode* a, SceneNode* b)
		{
			if (a->GetShader() == b->GetShader())
				return SceneNode::CompareByCameraDistance(a, b);
			return a->GetShader() < b->GetShader();
		});
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}


void Renderer::DrawOpaque() 
{
	SetProjectionMatrix();
	viewMatrix = camera->BuildViewMatrix();
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
}


void Renderer::DrawDepthNodes(Shader* shader)
{
	for (const auto& i : nodeList) {
		DrawNodeWithFallBack(i, shader);
	}
}



void Renderer::DrawTransparent()
{
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}


void Renderer::DrawNodes() 
{
	DrawOpaque();
	DrawTransparent();
}

void Renderer::DrawSkybox() {

	glDepthMask(GL_FALSE);
	BindShader(skyboxShader);
	UpdateShaderMatrices();
	skyQuad->Draw();
	glDepthMask(GL_TRUE);
}


void Renderer::CombineBuffers() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, postPFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "depthTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	skyQuad->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PostProcess() 
{
	BindShader(postProcessShader);
	glUniform1i(glGetUniformLocation(postProcessShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postPTex);
	skyQuad->Draw();
}


void Renderer::DrawPointLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointlightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(pointlightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();

	glUniformMatrix4fv(glGetUniformLocation(pointlightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();

	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	glDeleteTextures(1, &depthTex);
	glDeleteTextures(1, &depthFBO);
	glDeleteTextures(1, &postPFBO);
	glDeleteTextures(1, &postPTex);

	delete sceneShader;
	delete combineShader;
	delete postProcessShader;
	delete pointlightShader;
	delete shadowScene;
	delete sphere;
	delete[] pointLights;

	glDeleteTextures(1, &shadowFBO);
	glDeleteTextures(1, &shadowTex);
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
}

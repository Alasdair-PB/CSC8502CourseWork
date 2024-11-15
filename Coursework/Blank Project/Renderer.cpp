#include "Renderer.h"
#include <algorithm>

#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/Pathing.h"
#include "Particle.h"


Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{	
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	root = new SceneNode();

	if (!SetCubeMap() || !SetTerrain(root) || !SetWater(root) || !SetTree(root) || !SetFoliage(root)|| !SetRocks(root))
		return;	

	Vector3 cameraPos = Vector3(mapSize.x * 0.75f, 250.0f, mapSize.x * 0.75f);
	camera = new Camera(0.0f, 0.0f, Vector3(mapSize.x * 0.5f, 250.0f, mapSize.x * 0.5f));
	camera->GetPath()->AddCircuit(300, cameraPos);
	lastCameraPos = cameraPos;
	camera->GetPath()->SetPathing(false);
	GLuint* particleTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Rock_02_normal.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	fogTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "noise.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	Shader* particleShader = new Shader("particleVertex.glsl", "particleFragment.glsl");

	particleManager = ParticleManager(Vector3(mapSize.x * 0.5,165, mapSize.x * 0.5), particleTexture, particleShader);

	SetLights();
	SetFPSCharacter(root);
	SetProjectionMatrix();

	this->temperature = 30.0f;
	this->dt = 0;
	this->dtSeason = 0;
	this->currentFrame = 0;
	this->frameTime = 0.0f;

	SetupDepthbuffer();
	SetupFramebuffer();
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

bool pathing = true;
void Renderer::UpdateScene(float dt) 
{	

	UpdateFrameTime(dt);
	UpdateTemperature(dt);
	camera->UpdateCamera(dt);
	UpdateRunner();
	viewMatrix = camera->BuildViewMatrix();
	SetProjectionMatrix();
	root->Update(dt, camera->GetPosition());	
	
	
	camera->GetPath()->SetPathing(pathing);

}


Vector4 Mix(const Vector4& color1, const Vector4& color2, float ratio)
{
	ratio = std::clamp(ratio, 0.0f, 1.0f);
	return Vector4(
		color1.x * (1 - ratio) + color2.x * ratio,
		color1.y * (1 - ratio) + color2.y * ratio,
		color1.z * (1 - ratio) + color2.z * ratio,
		color1.w * (1 - ratio) + color2.w * ratio
	);
}


void Renderer::UpdateTemperature(float dt) 
{	
	this->dt += dt;
	this->dtSeason += dt;
	float nextTemperature = this->temperature;
	float deltaTemperature = 0.1f;

	float modedTime = this->dt;


	modedTime = fmod(modedTime, 100.0f);
	if (modedTime < 50.0f)
		nextTemperature -= deltaTemperature;
	else
		nextTemperature += deltaTemperature;


	particleManager.UpdateParticles(dt);

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_1))
		pathing = false;

	if ((nextTemperature < 0 && temperature > 0) || (nextTemperature > 0 && temperature < 0))
		this->dtSeason = 0;
	this->temperature = nextTemperature;

	float frozen = abs(abs(std::clamp(temperature - 10.0, -20.0, 0.0))) / 20;

	for (int i = 0; i < LIGHT_NUM; ++i) 
	{
		Light& l = pointLights[i];
		l.SetColour(Mix(Vector4(1.0, 0.6, 0.2, 1.0), Vector4(0.6, 0.7, 1.0, 1.0), frozen));
	}
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

	Vector3 terrainCenter = Vector3(mapSize.x * 0.5f, 0.0f, mapSize.x * 0.5f);
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), terrainCenter);
	viewMatrix = viewMatrix * Matrix4::Rotation(45, Vector3(0, 1, 0)) * Matrix4::Translation(Vector3(-mapSize.x * 0.5f, 0.0f, mapSize.x * 0.75f));
	float orthoSize = mapSize.x * 0.5f;
	projMatrix = Matrix4::Orthographic(-orthoSize, orthoSize, -orthoSize, orthoSize, 100.00f, 4200.0f);
	shadowMatrix = projMatrix * viewMatrix;


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
	DrawParticles();


	DeferredBufferWrite();
	DrawPointLights();

	DrawNodes();
	CombineBuffers();
	PostProcess();

	ClearNodeLists();
}

void Renderer::DrawParticles() 
{
	Shader* particleShader = particleManager.GetShader();
	GLuint* particleTexture = particleManager.GetTexture();
	const std::vector<Particle>& particles = particleManager.GetParticles();


	for (const Particle& particle : particleManager.GetParticles())
	{
		if (particle.life > 0.0f)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			BindShader(particleShader);
			glUniform1i(glGetUniformLocation(particleShader->GetProgram(), "sprite"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *particleTexture);

			glUniform3fv(glGetUniformLocation(particleShader->GetProgram(), "position"), 1, (float*)&particle.position);
			glUniform4fv(glGetUniformLocation(particleShader->GetProgram(), "colour"), 1, (float*)&particle.colour);

			particleManager.Draw();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}
	
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
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);
	
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(postProcessShader->GetProgram(), "diffuseTex"), 0);	
	glBindTexture(GL_TEXTURE_2D, postPTex);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(postProcessShader->GetProgram(), "depthTex"), 1);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1f(glGetUniformLocation(postProcessShader->GetProgram(), "fogDensity"), 100.0f);
	Vector3 fogColour = Vector3(1.5f, 1.5f, 1.5f);
	glUniform3fv(glGetUniformLocation(postProcessShader->GetProgram(), "fogColor"), 1, (float*)&fogColour);
	glUniform3fv(glGetUniformLocation(postProcessShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glUniform1i(glGetUniformLocation(postProcessShader->GetProgram(), "fogTexture"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, *fogTexture);

	camera->BuildViewMatrix();

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(postProcessShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);


	UpdateShaderMatrices();
	skyQuad->Draw();

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
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
	glDeleteTextures(1, fogTexture);


	delete sceneShader;
	delete combineShader;
	delete postProcessShader;
	delete pointlightShader;
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

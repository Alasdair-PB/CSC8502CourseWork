#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "Terrain.h"
#include "Water.h"
#include "../nclgl/Light.h"
#include "../nclgl/Material.h"
#include "../nclgl/MeshMaterial.h"

#include <algorithm>

const int LIGHT_NUM = 32;


Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	root = new SceneNode();

	if (!SetCubeMap() || !SetTerrain(root) || !SetWater(root) || !SetTree(root)) //
		return; // Will throw errors as deleting shaders that have not been assigned on ~Renderer

	// Map size setup in SetTerrain
	light = new Light(mapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 0.8f, 0.5f, 1), mapSize.x * 0.5f);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	camera = new Camera(-45.0f, 0.0f, mapSize * Vector3(0.5f, 5.0f, 0.5f));

	this->dt = 0;
	// Bind depth Buffer---------------------------------------------------------------------------------
	glGenFramebuffers(1, &depthFBO);
	glGenTextures(1, &depthTex);

	glBindTexture(GL_TEXTURE_2D, depthTex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// End bind depth buffer---------------------------------------------------------------------------------

	// Shadow Mapping-------------------------------------------------------------------------------------
	pointLights = new Light[LIGHT_NUM];

	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
		l.SetPosition(Vector3(rand() % (int)mapSize.x,
			150.0f,
			rand() % (int)mapSize.z));

		l.SetColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));
		l.SetRadius(250.0f + (rand() % 250));
	}

	sceneShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl"); // reused
	pointlightShader = new Shader("pointlightvertex.glsl", "pointlightfrag.glsl");
	combineShader = new Shader("combinevert.glsl", "combinefrag.glsl");

	if (!sceneShader->LoadSuccess() || !pointlightShader->LoadSuccess() || !combineShader->LoadSuccess()) 
		return;

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

	// Generate our scene depth texture
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex); 
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_CULL_FACE);
	//End shadow mapping-----------------------------------------------------------------------


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
	glDeleteTextures(1, &depthTex);
	glDeleteTextures(1, &depthFBO);


	// Shadow mapping ----------------------------------------------------------
	delete sceneShader;
	delete combineShader;
	delete pointlightShader;
	delete sphere;
	delete[] pointLights;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	// End Shadow mapping -------------------------------------------------------

}


void Renderer::UpdateScene(float dt) 
{	
	this->dt += dt;
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(dt);

}

void Renderer::RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BuildNodeLists(root);
	SortNodeLists();
	DrawSkybox();

	// Write to depth texture------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 
    DrawOpaque();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// ------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	DrawNodes();
	UpdateShaderMatrices();
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 

	modelMatrix.ToIdentity();
	//viewMatrix = camera->BuildViewMatrix();
	//projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);


	//DrawPointLights();
   //CombineBuffers();	

	//DrawTransparent();
	//UpdateShaderMatrices();
	DrawNodes();


	ClearNodeLists();


}


void Renderer::DrawNode(SceneNode* n) {

	if (n->GetMesh() && n->GetShader()) {
		if (currentShader != n->GetShader()) {
			currentShader = n->GetShader();
			BindShader(currentShader);
			UpdateShaderMatrices();
		}

		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		Material* material = n->GetMaterial();
		bool renderFlag = false;
		bool faceCulling = true;

		if (material) 
		{
			const auto& propertyMaps = material->GetProperties();

			for (const auto& propertyMap : propertyMaps) {
				for (const auto& [outerKey, innerMap] : propertyMap) {
					for (const auto& [innerKey, value] : innerMap) {
						int index = 0;
						std::visit([&](auto&& val) {
							using T = std::decay_t<decltype(val)>;
							GLint location = glGetUniformLocation(currentShader->GetProgram(), innerKey.c_str());

							if constexpr (std::is_same_v<T, GLuint>) 
							{
								glActiveTexture(GL_TEXTURE0 + index);
								glBindTexture(GL_TEXTURE_2D, val); 
								glUniform1i(location, index);
							}
							else if constexpr (std::is_same_v<T, Vector4>) 
							{
								glUniform4fv(location, 1, reinterpret_cast<const float*>(&val));
							}
							else if constexpr (std::is_same_v<T, Vector3>) 
							{
								glUniform3fv(location, 1, reinterpret_cast<const float*>(&val));
							}
							else if constexpr (std::is_same_v<T, Matrix4>) 
							{
								glUniformMatrix4fv(location, 1, false, val.values);
							}
							else if constexpr (std::is_same_v<T, int>) 
							{
								glUniform1i(location, val);
							}
							else if constexpr (std::is_same_v<T, float>)
							{
								glUniform1f(location, val);
							}
							else if constexpr (std::is_same_v<T, std::vector<GLuint>>) 
							{
								Mesh* myMesh = n->GetMesh();
								for (int i = 0; i < myMesh->GetSubMeshCount(); ++i) 
								{
									glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
									glActiveTexture(GL_TEXTURE0);
									glBindTexture(GL_TEXTURE_2D, val[i]);
									//myMesh->DrawSubMesh(i);
								}
							}
							else if constexpr (std::is_same_v<T, Material::WorldValue>) 
							{
								switch (val) {
								case Material::CameraPosition:
									glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
									break;
								case Material::CubeMap:
									glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
									glActiveTexture(GL_TEXTURE2);
									glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
									break;
								case Material::FarPlane:
									glUniform1f(location, 15000.0f);
									break;

								case Material::ProjMatrix:
									glUniformMatrix4fv(location, 1, false, (float*)&this->projMatrix);
									break;

								case Material::ViewMatrix:
									glUniformMatrix4fv(location, 1, false, (float*)&camera->BuildViewMatrix());
									break;
								case Material::DeltaTime:
									glUniform1f(location, this->dt);
									break;
								case Material::Dimensions:
									glUniform2fv(location,1, (float*)&( Vector2(width, height)));
									break;
								case Material::DualFace:
									faceCulling = false;
									break;

								case Material::DepthTexture:									
									glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 3);
									glActiveTexture(GL_TEXTURE3);
									glBindTexture(GL_TEXTURE_2D, depthTex);

									break;
								case Material::LightRender:
									renderFlag = true;
									break;
								default:
									break;
								}
							}
						}, value);
					}
				}
			}
		}
		if (renderFlag) {

			SetShaderLight(*light);
			UpdateShaderMatrices();
		}

		if (faceCulling == false)
			glDisable(GL_CULL_FACE);

		n->Draw(*this);

		if (faceCulling == false)
			glEnable(GL_CULL_FACE);

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
	for (const auto& i : nodeList) {
		DrawNode(i);
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


bool Renderer::SetTerrain(SceneNode* root)
{
	GLuint* newTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	GLuint* newBumpTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!newTexture || !newBumpTexture)
		return false;

	SetTextureRepeating(*newTexture, true);
	SetTextureRepeating(*newBumpTexture, true);

	Shader* newTerrainShader = new Shader("bumpvertex.glsl", "bumpfragment.glsl");

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




bool Renderer::SetTree(SceneNode* root)
{
	Mesh* myMesh = Mesh::LoadFromMeshFile("Leaves.msh");
	Shader* newShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl", "Geometry.glsl");
	shader.emplace_back(newShader);

	if (!myMesh)
		return false;

	SceneNode* tree = new SceneNode(myMesh, Vector4(0, 0, 0, 1));
	tree->SetShader(newShader);

	MeshMaterial* material = new MeshMaterial("Leaves.mat");
	std::vector<GLuint> textures(myMesh->GetSubMeshCount());

	for (int i = 0; i < myMesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;

		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

		textures[i] = texID; 
	}

	tree->GetMaterial()->AddProperty("Diffuse", textures);
	tree->GetMaterial()->AddProperty("dt", Material::DeltaTime);
	tree->GetMaterial()->AddProperty("cameraPosition", Material::CameraPosition);

	tree->GetMaterial()->AddProperty("faceCullOff", Material::DualFace);

	tree->GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	tree->GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);

	delete material;

	tree->SetTransform(Matrix4::Translation(Vector3(mapSize.x * 0.5f, 165, mapSize.x * 0.5f)));
	tree->SetModelScale(Vector3(25, 25, 25));




	Mesh* myTrunkMesh = Mesh::LoadFromMeshFile("Trunk.msh");
	Shader* newTrunkShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	shader.emplace_back(newTrunkShader);

	if (!myTrunkMesh)
		return false;

	SceneNode* trunk = new SceneNode(myTrunkMesh, Vector4(0, 0, 0, 1));
	trunk->SetShader(newTrunkShader);

	MeshMaterial* materialTrunk = new MeshMaterial("Trunk.mat");
	std::vector<GLuint> texturesTrunk(myTrunkMesh->GetSubMeshCount());

	for (int i = 0; i < myTrunkMesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = materialTrunk->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;

		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

		texturesTrunk[i] = texID;
	}

	trunk->GetMaterial()->AddProperty("Diffuse", texturesTrunk);
	trunk->GetMaterial()->AddProperty("dt", Material::DeltaTime);

	delete materialTrunk;

	trunk->SetTransform(Matrix4::Translation(Vector3(mapSize.x * 0.5f, 165, mapSize.x * 0.5f)));
	trunk->SetModelScale(Vector3(25, 25, 25));





	root->AddChild(tree);
	root->AddChild(trunk);

	return true;
}

bool Renderer::SetWater(SceneNode* root)
{
	GLuint* newTexture = new GLuint(SOIL_load_OGL_texture(TEXTUREDIR "water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	Shader* waterShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl"); 
	SetTextureRepeating(*newTexture, true);

	if (!waterShader->LoadSuccess())
		return false;

	texture.push_back(newTexture);
	shader.emplace_back(waterShader);

	Water* water = new Water(*newTexture, mapSize.x);
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

void Renderer::CombineBuffers() {
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

	skyQuad->Draw();
}


void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0,
		format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
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

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(pointlightShader->GetProgram(), "inverseProjView"),
		1, false, invViewProj.values);

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
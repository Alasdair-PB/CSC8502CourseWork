#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/HeightMap.h"
#include "Particle.h"



class Camera;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

protected:
	void GLEnablers();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();

	void SetNodePosition(SceneNode** nodes, size_t count);
	bool SetTerrain(SceneNode* root);
	bool SetWater(SceneNode* root);
	bool SetTree(SceneNode* root);
	bool SetFoliage(SceneNode* root);
	bool SetRocks(SceneNode* root);
	bool SetFPSCharacter(SceneNode* root);
	bool SetCubeMap();
	void SetDefaults();
	void SetUpCamera();
	bool SetUpLightingShaders();

	Matrix4* GetNodePositions(size_t count, bool alignToFace);

	void UpdateTemperature(float dt);
	void UpdateFrameTime(float dt);
	void UpdateRunner();
	void UpdateLightColourByTemp();
	void KeyBoardBinds();
	void ParticleSetUp();

	void SetWorldValues(bool* renderFlag, bool* faceCulling, bool* tessFalg, int* index, int* batchDraws, Material::WorldValue val, GLint location);
	void SetTextureParam(int* index, GLint location, GLuint val);
	void DepthBufferWrite();
	void DeferredBufferWrite();
	void ShadowBufferWrite();

	void DrawSkybox();
	void DrawParticles();
	void DrawNodes();
	void DrawTransparent();
	void DrawDepthNodes(Shader* shader);
	void DrawNodeWithFallBack(SceneNode* n, Shader* shader);
	void DrawNode(SceneNode* n);
	void DrawOpaque();
	void DrawPointLights();

	void SetProjectionMatrix();

	void CombineBuffers();
	void GenerateScreenTexture(GLuint& into, bool depth = false);

	void PostProcess();
	void SetLights();

	void SetupFramebuffer();
	void SetUpShadowMapBuffer();
	void SetupDepthbuffer();
	void SetupDeferredbuffer();

	#define SHADOWSIZE 2048

	SceneNode* root;
	SceneNode* runningGuy;
	Camera* camera;	
	Light* light;

	ParticleManager particleManager;

	const int LIGHT_NUM = 32;
	const int foliageCount = 300;

	float dt;
	float dtSeason;
	float temperature;
	float currentFrame;
	float frameTime;
	const GLint patchverts = 4;

	Vector3 lastCameraPos;

	Shader* currentShader;

	Shader* skyboxShader;
	Shader* sceneShader;
	Shader* pointlightShader;
	Shader* combineShader;
	Shader* postProcessShader;
	Shader* shadowScene;
	Shader* fallBackShader;

	HeightMap* heightMap;

	GLuint cubeMap;
	GLuint currentTexture;
	GLuint currentBumpMap;

	GLuint depthFBO;
	GLuint depthTex;

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint postPFBO;         
	GLuint postPTex;

	GLuint* fogTexture;

	GLuint bufferFBO; 
	GLuint bufferColourTex; 
	GLuint bufferNormalTex; 
	GLuint bufferDepthTex; 

	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex; 

	Mesh* skyQuad;
	Mesh* sphere; 

	vector<Shader*> shader;
	vector<GLuint*> texture;
	vector<GLuint*> textureBump;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	Light* pointLights; 
	Vector3 mapSize;

};

#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/HeightMap.h"


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

	bool SetTerrain(SceneNode* root);
	bool SetWater(SceneNode* root);
	bool SetTree(SceneNode* root);
	bool SetFPSCharacter(SceneNode* root);
	bool SetCubeMap();

	void UpdateTemperature(float dt);
	void UpdateFrameTime(float dt);

	void SetWorldValues(bool* renderFlag, bool* faceCulling, bool* tessFalg, Material::WorldValue val, GLint location);
	void DepthBufferWrite();
	void DeferredBufferWrite();

	void DrawSkybox();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	void DrawOpaque();
	void DrawTransparent();
	void DrawPointLights();
	void DrawNodeReflective();

	void CombineBuffers();
	void GenerateScreenTexture(GLuint& into, bool depth = false);


	void PostProcess();
	void SetLights();

	void SetupFramebuffer();
	void SetupDepthbuffer();
	void SetupDeferredbuffer();



	SceneNode* root;
	Camera* camera;	
	Light* light;

	const int LIGHT_NUM = 32;

	float dt;
	float dtSeason;
	float temperature;
	float currentFrame;
	float frameTime;
	const GLint patchverts = 4;

	Shader* skyboxShader;
	Shader* currentShader;
	Shader* sceneShader;
	Shader* pointlightShader;
	Shader* combineShader;
	Shader* postProcessShader;

	GLuint cubeMap;
	GLuint currentTexture;
	GLuint currentBumpMap;

	GLuint depthFBO;
	GLuint depthTex;

	GLuint postPFBO;         
	GLuint postPTex;

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

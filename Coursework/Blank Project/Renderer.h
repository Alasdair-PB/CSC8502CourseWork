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

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();

	bool SetTerrain(SceneNode* root);
	bool SetWater(SceneNode* root);
	bool SetTree(SceneNode* root);
	void UpdateTemperature(float dt);
	void DrawSkybox();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	void DrawOpaque();
	void DrawTransparent();
	void CombineBuffers();
	bool SetCubeMap();
	void DepthBufferWrite();
	void DeferredBufferWrite();
	void DrawPointLights();
	void DrawNodeReflective();
	void GenerateScreenTexture(GLuint& into, bool depth = false);

	SceneNode* root;
	Camera* camera;	
	Light* light;

	float dt;
	float dtSeason;
	float temperature;
	const GLint patchverts = 4;

	Shader* skyboxShader;
	Shader* currentShader;
	Shader* sceneShader;
	Shader* pointlightShader;
	Shader* combineShader;

	GLuint cubeMap;
	GLuint currentTexture;
	GLuint currentBumpMap;

	GLuint depthFBO;
	GLuint depthTex;

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

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
	bool SetCubeMap();

	void DrawSkybox();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	void DrawOpaque();

	void DrawNodeReflective();

	SceneNode* root;
	Camera* camera;	
	Light* light;


	Shader* skyboxShader;

	Shader* currentShader;
	GLuint currentTexture;
	GLuint currentBumpMap;
	GLuint depthFBO;
	GLuint depthTex;

	Mesh* skyQuad;
	GLuint cubeMap;

	vector<Shader*> shader;
	vector<GLuint*> texture;
	vector<GLuint*> textureBump;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	Vector3 mapSize;

};

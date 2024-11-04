#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/HeightMap.h"

#include "Terrain.h"


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

	SceneNode* root;
	Camera* camera;	
	Light* light;

	Shader* currentShader;
	Shader* waterShader;
	Shader* terrainShader;

	Shader* skyboxShader;
	Mesh* skyQuad;

	GLuint texture;
	GLuint textureBump;
	GLuint cubeMap;


	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	Vector3 mapSize;

};

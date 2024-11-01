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
	void SetTerrain(SceneNode* root);
	void SetWater(SceneNode* root);

	void DrawNodes();
	void DrawNode(SceneNode* n);
	SceneNode* root;
	Camera* camera;
	Shader* waterShader;
	Shader* terrainShader;
	GLuint texture;
	GLuint textureBump;


	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

};

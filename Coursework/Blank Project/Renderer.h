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


	void DrawSkybox();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	void DrawOpaque();
	void DrawTransparent();
	void CombineBuffers();
	bool SetCubeMap();

	void DrawPointLights();
	void DrawNodeReflective();
	void GenerateScreenTexture(GLuint& into, bool depth = false);

	SceneNode* root;
	Camera* camera;	
	Light* light;
	float dt;


	Shader* skyboxShader;

	Shader* currentShader;
	GLuint currentTexture;
	GLuint currentBumpMap;
	GLuint depthFBO;
	GLuint depthTex;

	Mesh* skyQuad;
	Mesh* sphere; // Light volume

	GLuint cubeMap;

	vector<Shader*> shader;
	vector<GLuint*> texture;
	vector<GLuint*> textureBump;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;



	Shader* sceneShader; // Shader to fill our GBuffers
	Shader* pointlightShader; // Shader to calculate lighting
	Shader* combineShader; // Shader to combine buffers

	GLuint bufferFBO; // FBO for our G-Buffer pass
	GLuint bufferColourTex; // Albedo texture
	GLuint bufferNormalTex; // Normals texture
	GLuint bufferDepthTex; // Depth texture

	GLuint pointLightFBO; // FBO for lighting pass
	GLuint lightDiffuseTex; // Store diffuse lighting
	GLuint lightSpecularTex; // Store specular lighting
	Light* pointLights; // Array of lighting data


	Vector3 mapSize;

};

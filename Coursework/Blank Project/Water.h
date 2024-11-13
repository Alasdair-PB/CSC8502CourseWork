#pragma once
#include "..\nclgl\scenenode.h"

class Water : public SceneNode {
public:
	Water(GLuint texture, GLuint bumpTex, GLuint iceTex, GLuint iceTexBump, float hSize);

	~Water(void) {

		delete quad;

	};
	void Update(float dt, Vector3 cameraPos) override;

protected:
	SceneNode* water;
	Mesh* quad;
};
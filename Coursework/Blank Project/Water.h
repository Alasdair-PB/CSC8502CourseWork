#pragma once
#include "..\nclgl\scenenode.h"

class Water : public SceneNode {
public:
	Water(GLuint texture, GLuint bumpTex, float hSize);

	~Water(void) {

		delete quad;

	};
	void Update(float dt) override;

protected:
	SceneNode* water;
	Mesh* quad;
};
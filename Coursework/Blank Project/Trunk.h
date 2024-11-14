#pragma once
#include "..\nclgl\scenenode.h"
#include "MshObject.h";

class Trunk : public MshObject {
public:
	Trunk(float hSize, GLuint icicleMask, GLuint iceTexture, GLuint woodTexture, GLuint woodNormal, GLuint iceBumpTex);

	~Trunk(void) {

		delete mesh;

	};
	void Update(float dt, Vector3 cameraPos) override;

protected:
	Mesh* mesh;
};
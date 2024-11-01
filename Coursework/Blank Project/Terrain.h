#pragma once
#include "..\nclgl\scenenode.h"

class Terrain : public SceneNode {
public:
	Terrain(GLuint texture);
	~Terrain(void) {};
	void Update(float dt) override;

protected:
	SceneNode* ground;
};
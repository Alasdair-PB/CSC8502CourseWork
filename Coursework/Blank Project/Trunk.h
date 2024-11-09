#pragma once
#include "..\nclgl\scenenode.h"
#include "MshObject.h";

class Trunk : public MshObject {
public:
	Trunk(float hSize);

	~Trunk(void) {

		delete mesh;

	};
	void Update(float dt) override;

protected:
	SceneNode* trunk;
	Mesh* mesh;
};
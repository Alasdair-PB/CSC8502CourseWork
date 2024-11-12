#pragma once
#include "..\nclgl\scenenode.h";
#include "MshObject.h";


class Leaves : public MshObject {
public:
	Leaves(float hSize);

	~Leaves(void) {

		delete mesh;

	};
	void Update(float dt, Vector3 cameraPos) override;

protected:
	SceneNode* leaves;
	Mesh* mesh;
};
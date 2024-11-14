#pragma once
#include "..\nclgl\scenenode.h";
#include "MshObject.h";


class Foliage : public MshObject {
public:
	Foliage();

	~Foliage(void) {

		delete mesh;

	};
	void Update(float dt, Vector3 cameraPos) override;

protected:
	SceneNode* foliage;
	Mesh* mesh;
};
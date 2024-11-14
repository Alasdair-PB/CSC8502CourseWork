#pragma once
#include "..\nclgl\scenenode.h";
#include "MshObject.h";


class Rock : public MshObject {
public:
	Rock();

	~Rock(void) {

		delete mesh;

	};
	void Update(float dt, Vector3 cameraPos) override;

protected:
	SceneNode* rock;
	Mesh* mesh;
};
#pragma once
#include "..\nclgl\scenenode.h";
#include "MshObject.h";


class Rock : public MshObject {
public:
	Rock(Matrix4* offsets);

	~Rock(void) {

		delete mesh;

	};
	void Update(float dt, Vector3 cameraPos) override;

protected:
	SceneNode* rock;
	Mesh* mesh;
	Matrix4* offsets;
};
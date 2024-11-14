#pragma once
#include "..\nclgl\scenenode.h";
#include "MshObject.h";


class Leaves : public MshObject {
public:
	Leaves(Matrix4* offsetBatch);

	~Leaves(void) {

		delete mesh;

	};
	void Update(float dt, Vector3 cameraPos) override;

protected:
	SceneNode* leaves;
	Mesh* mesh;
	Matrix4* offsetBatch;
};
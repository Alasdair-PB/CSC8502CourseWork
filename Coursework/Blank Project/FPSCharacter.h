#pragma once
#include "..\nclgl\scenenode.h";
#include "MshObject.h";

class FpsCharacter : public MshObject 
{

public:
	FpsCharacter(float hSize);

	~FpsCharacter(void) {
		delete fpsCharater;
		delete anim;
	};

	void Update(float dt, Vector3 cameraPos) override;

protected:
	Mesh* fpsCharater;
	MeshAnimation* anim;
};
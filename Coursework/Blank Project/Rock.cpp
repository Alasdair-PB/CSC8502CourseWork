#include "Rock.h"


Rock::Rock(Matrix4* offsets, GLuint* rockBump)
{

	SetFromSubMesh("RockAfbx");
	GetMaterial()->AddProperty("batch", offsets);
	GetMaterial()->AddProperty("rockBump", rockBump);

	GetMaterial()->AddProperty("cameraPosition", Material::CameraPosition);
	GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);
	GetMaterial()->AddProperty("batchSize", Material::BatchSize100);


	SetColour(Vector4(0, 0, 0, 1));
	SetModelScale(Vector3(25, 25, 25));
}



void Rock::Update(float dt, Vector3 cameraPos)
{
	SceneNode::Update(dt, cameraPos);
}

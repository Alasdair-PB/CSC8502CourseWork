#include "Foliage.h"


Foliage::Foliage(Matrix4* offsets)
{
	SetFromSubMesh("GrassBlade");

	GetMaterial()->AddProperty("wiggleIntensity", 0.5f);
	GetMaterial()->AddProperty("wiggleFrequency", 2.5f);
	GetMaterial()->AddProperty("batch", offsets);


	GetMaterial()->AddProperty("dt", Material::DeltaTime);
	GetMaterial()->AddProperty("temperature", Material::Temperature);
	GetMaterial()->AddProperty("cameraPosition", Material::CameraPosition);
	GetMaterial()->AddProperty("faceCullOff", Material::DualFace);
	GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);
	GetMaterial()->AddProperty("dtSeason", Material::DeltaTimeSeason);
	GetMaterial()->AddProperty("batchSize", Material::BatchSize300);



	SetColour(Vector4(0, 0, 0, 1));
	SetModelScale(Vector3(25, 25, 25));
}



void Foliage::Update(float dt, Vector3 cameraPos)
{
	SceneNode::Update(dt, cameraPos);
}

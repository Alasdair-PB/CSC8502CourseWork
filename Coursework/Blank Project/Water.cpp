#include "Water.h"

Water::Water(GLuint texture, GLuint bumpTex, float hSize)
	
{
	quad = Mesh::GenerateQuad();

	SetMesh(quad);
	SetModelScale(Vector3(hSize * 0.5f, hSize * 0.5f, hSize * 0.5f));
	SetColour(Vector4(0, 0, 1, 0.5f));

	GetMaterial()->AddProperty("diffuseTex", texture);
	GetMaterial()->AddProperty("bumpTex", bumpTex);
	GetMaterial()->AddProperty("transparency", 0.8f);
	GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);
	GetMaterial()->AddProperty("foamCutoff", 0.4f);
	GetMaterial()->AddProperty("foamSpeed", 4.0f);
	GetMaterial()->AddProperty("scrollSpeed", 0.01f);	
	GetMaterial()->AddProperty("waterScale", 15.0f);

	GetMaterial()->AddProperty("tessBuffer", Material::TesselationBuffer);
	//GetMaterial()->AddProperty("tessLevelInner", 0.0f); // Without tess shader will break everything!!!
	//GetMaterial()->AddProperty("tessLevelOuter", 0.0f);
	GetMaterial()->AddProperty("dimensions", Material::Dimensions);
	GetMaterial()->AddProperty("temperature", Material::Temperature);
	GetMaterial()->AddProperty("dt", Material::DeltaTime);

	GetMaterial()->AddProperty("cameraPos", Material::CameraPosition);
	GetMaterial()->AddProperty("cubeMap", Material::CubeMap);
	GetMaterial()->AddProperty("depthTex", Material::DepthTexture);

	SetTransform(Matrix4::Translation(Vector3(hSize * 0.5f, 100, hSize * 0.5f)) * Matrix4::Rotation(270.0f, Vector3(1, 0, 0)));
	SetBoundingRadius(15.0f);
}


void Water::Update(float dt) {
	SceneNode::Update(dt);
}
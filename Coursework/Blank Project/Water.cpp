#include "Water.h"

Water::Water(GLuint texture, GLuint bumpTex, GLuint iceTex, GLuint iceTexBump, float hSize)
	
{
	quad = Mesh::GenerateQuad();

	SetMesh(quad);
	SetModelScale(Vector3(hSize * 0.5f, hSize * 0.5f, hSize * 0.5f));
	SetColour(Vector4(0, 0, 1, 0.5f));

	GetMaterial()->AddProperty("diffuseTex", texture);
	GetMaterial()->AddProperty("iceTex", iceTex);
	GetMaterial()->AddProperty("iceTexBump", iceTexBump);
	GetMaterial()->AddProperty("bumpTex", bumpTex);
	GetMaterial()->AddProperty("cubeMap", Material::CubeMap);
	GetMaterial()->AddProperty("depthTex", Material::DepthTexture);

	GetMaterial()->AddProperty("transparency", 0.8f);
	GetMaterial()->AddProperty("projMatrix", Material::ProjMatrix);
	GetMaterial()->AddProperty("viewMatrix", Material::ViewMatrix);
	GetMaterial()->AddProperty("foamCutoff", 0.4f);
	GetMaterial()->AddProperty("foamSpeed", 4.0f);
	GetMaterial()->AddProperty("scrollSpeed", 0.005f);	
	GetMaterial()->AddProperty("waterScale", 25.0f);

	GetMaterial()->AddProperty("dimensions", Material::Dimensions);
	GetMaterial()->AddProperty("temperature", Material::Temperature);
	GetMaterial()->AddProperty("dt", Material::DeltaTime);

	GetMaterial()->AddProperty("cameraPos", Material::CameraPosition);

	SetTransform(Matrix4::Translation(Vector3(hSize * 0.5f, 100, hSize * 0.5f)) * Matrix4::Rotation(270.0f, Vector3(1, 0, 0)));
	SetBoundingRadius(15.0f);	
	
	GetMaterial()->AddProperty("tessBuffer", Material::TessQuad);
	GetMaterial()->AddProperty("frequency", 0.2f);
	GetMaterial()->AddProperty("tessLevelInner", 25.0f); 
	GetMaterial()->AddProperty("tessLevelOuter", 2.0f);
}



void Water::Update(float dt, Vector3 cameraPos)
{
	SceneNode::Update(dt, cameraPos);
}

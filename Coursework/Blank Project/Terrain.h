#pragma once
#include "..\nclgl\scenenode.h"

class Terrain : public SceneNode 
{
	public:
		Terrain(GLuint* texture, GLuint* textureBump, GLuint* snowTex, GLuint* snowBump);
		~Terrain(void) {};
		void Update(float dt, Vector3 cameraPos) override;

	protected:
		SceneNode* ground;
		GLuint CreateTextureAtlas(const std::vector<GLuint>& textures, int singleTextureWidth, int singleTextureHeight, int atlasCols);
};
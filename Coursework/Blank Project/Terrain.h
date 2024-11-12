#pragma once
#include "..\nclgl\scenenode.h"

class Terrain : public SceneNode 
{
	public:
		Terrain(GLuint texture, GLuint textureBump);
		~Terrain(void) {};
		void Update(float dt, Vector3 cameraPos) override;

		Vector3 GetMapSize(){ return mapSize; }

	protected:
		SceneNode* ground;
		Vector3 mapSize;
		GLuint CreateTextureAtlas(const std::vector<GLuint>& textures, int singleTextureWidth, int singleTextureHeight, int atlasCols);
};
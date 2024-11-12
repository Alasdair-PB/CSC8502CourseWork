#pragma once
#include "../nclgl/MeshMaterial.h"
#include "..\nclgl\scenenode.h"


class MshObject : public SceneNode
{
	protected:

		void SetFromSubMesh(std::string name) {
			mesh = Mesh::LoadFromMeshFile(name + ".msh");
			SetMesh(mesh);

			MeshMaterial* material = new MeshMaterial(name + ".mat");
			std::vector<GLuint> textures(mesh->GetSubMeshCount());

			for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
				const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);
				const string* filename = nullptr;
				matEntry->GetEntry("Diffuse", &filename);
				string path = TEXTUREDIR + *filename;

				GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
				textures[i] = texID;
			}

			if (textures.size() > 1)
				GetMaterial()->AddProperty("Diffuse", textures);
			else 
				GetMaterial()->AddProperty("Diffuse", textures[0]);

			delete material;

		}
};
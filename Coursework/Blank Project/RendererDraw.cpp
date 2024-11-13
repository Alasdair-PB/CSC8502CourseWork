#include "Renderer.h"
#include "../nclgl/Material.h"
#include "../nclgl/MeshAnimation.h"

void Renderer::DrawNodeWithFallBack(SceneNode* n, Shader* shader)
{
	if (n->GetMesh()) {
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
		UpdateShaderMatrices();
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
		n->Draw(*this);
	}
}

void Renderer::DrawNode(SceneNode* n) {

	if (n->GetMesh() && n->GetShader()) {
		if (currentShader != n->GetShader()) {
			currentShader = n->GetShader();
			BindShader(currentShader);
			UpdateShaderMatrices();
		}

		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		Material* material = n->GetMaterial();

		bool renderFlag = false;
		bool faceCulling = true;
		bool tessFalg = false;

		if (!material)
			return;
		
		const auto& propertyMaps = material->GetProperties();
		int index = 0;

		for (const auto& propertyMap : propertyMaps) {
			for (const auto& [outerKey, innerMap] : propertyMap) {
				for (const auto& [innerKey, value] : innerMap) {
					std::visit([&](auto&& val) {
						using T = std::decay_t<decltype(val)>;
						GLint location = glGetUniformLocation(currentShader->GetProgram(), innerKey.c_str());

						if constexpr (std::is_same_v<T, GLuint>)
						{
							glActiveTexture(GL_TEXTURE0 + index);
							glBindTexture(GL_TEXTURE_2D, val);
							glUniform1i(location, index);
							index++;
						}
						else if constexpr (std::is_same_v<T, Vector4>)
						{
							glUniform4fv(location, 1, reinterpret_cast<const float*>(&val));
						}
						else if constexpr (std::is_same_v<T, Vector3>)
						{
							glUniform3fv(location, 1, reinterpret_cast<const float*>(&val));
						}
						else if constexpr (std::is_same_v<T, Matrix4>)
						{
							glUniformMatrix4fv(location, 1, false, val.values);
						}
						else if constexpr (std::is_same_v<T, MeshAnimation*>)
						{
							Mesh* myMesh = n->GetMesh();

							while (frameTime < 0.0f)
							{
								int lastCurrentFrame = currentFrame + 1;
								currentFrame = lastCurrentFrame % val->GetFrameCount();
								frameTime += 1.0f / val->GetFrameRate();
							}
							vector <Matrix4> frameMatrices;
							const Matrix4* invBindPose = myMesh->GetInverseBindPose();
							const Matrix4* frameData = val->GetJointData(currentFrame);

							for (unsigned int i = 0; i < myMesh->GetJointCount(); ++i) {
								frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
							}
							int j = glGetUniformLocation(currentShader->GetProgram(), "joints");
							glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
						}
						else if constexpr (std::is_same_v<T, int>)
						{
							glUniform1i(location, val);
						}
						else if constexpr (std::is_same_v<T, float>)
						{
							glUniform1f(location, val);
						}
						else if constexpr (std::is_same_v<T, std::vector<GLuint>>)
						{
							Mesh* myMesh = n->GetMesh();
							glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), index);
							for (int i = 0; i < myMesh->GetSubMeshCount(); ++i)
							{
								glActiveTexture(GL_TEXTURE0 + index);
								glBindTexture(GL_TEXTURE_2D, val[i]);
								myMesh->DrawSubMesh(i);
							}
							index++;
						}
						else if constexpr (std::is_same_v<T, Material::WorldValue>)
						{
							SetWorldValues(&renderFlag, &faceCulling, &tessFalg, &index, val, location);
						}
						}, value);
				}
			}
		}
		
		if (renderFlag) 
			SetShaderLight(*light);

		if (faceCulling == false)
			glDisable(GL_CULL_FACE);

		if (tessFalg)
			n->Draw(*this, GL_PATCHES);
		else
			n->Draw(*this);

		if (faceCulling == false)
			glEnable(GL_CULL_FACE);

	}
}


void Renderer::SetWorldValues(bool* renderFlag, bool* faceCulling, bool* tessFalg, int* index, Material::WorldValue val, GLint location)
{
	switch (val) {
		case Material::CameraPosition:
			glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
			break;
		case Material::CubeMap:
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), *index);
			glActiveTexture(GL_TEXTURE0 + *index);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
			*index += 1;
			break;
		case Material::FarPlane:
			glUniform1f(location, 15000.0f);
			break;
		case Material::ProjMatrix:
			glUniformMatrix4fv(location, 1, false, (float*)&this->projMatrix);
			break;
		case Material::ViewMatrix:
			glUniformMatrix4fv(location, 1, false, (float*)&camera->BuildViewMatrix());
			break;
		case Material::TessQuad:
			glPatchParameteri(GL_PATCH_VERTICES, 4);
			*tessFalg = true;
			break;
		case Material::TessTri:
			glPatchParameteri(GL_PATCH_VERTICES, 3);
			*tessFalg = true;
			break;
		case Material::Temperature:
			glUniform1f(location, temperature);
			break;
		case Material::DeltaTime:
			glUniform1f(location, this->dt);
			break;
		case Material::DeltaTimeSeason:
			glUniform1f(location, this->dtSeason);
			break;
		case Material::Dimensions:
			glUniform2fv(location, 1, (float*)&(Vector2(width, height)));
			break;
		case Material::DualFace:
			faceCulling = false;
			break;

		case Material::DepthTexture:
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), *index);
			glActiveTexture(GL_TEXTURE0 + *index);
			glBindTexture(GL_TEXTURE_2D, depthTex);
			*index += 1;
			break;

		case Material::ShadowMap:
			// Changed to hard setting while debugging
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), *index);
			glActiveTexture(GL_TEXTURE0 + *index);
			glBindTexture(GL_TEXTURE_2D, shadowTex);
			*index += 1;
			break;

		case Material::LightRender:
			*renderFlag = true;
			break;
		default:
			break;
	}

}


#include "Renderer.h"
#include "../nclgl/Material.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/Light.h"
#include "Particle.h"

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

void Renderer::DrawNode(SceneNode* n) 
{

	if (n->GetMesh() && n->GetShader()) {
		if (currentShader != n->GetShader()) {
			currentShader = n->GetShader();
			BindShader(currentShader);
			UpdateShaderMatrices();
		}
		Material* material = n->GetMaterial();

		bool renderFlag = false;
		bool faceCulling = true;
		bool tessFalg = false;
		int batchDraws = 1;

		if (!material)
			return;
		

		const auto& propertyMaps = material->GetProperties();
		int index = 0;
		Matrix4* batchOffsets = NULL;
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());


		for (const auto& propertyMap : propertyMaps) {
			for (const auto& [outerKey, innerMap] : propertyMap) {
				for (const auto& [innerKey, value] : innerMap) {
					std::visit([&](auto&& val) {
						using T = std::decay_t<decltype(val)>;
						GLint location = glGetUniformLocation(currentShader->GetProgram(), innerKey.c_str());

						if constexpr (std::is_same_v<T, GLuint*>)
						{
							glActiveTexture(GL_TEXTURE0 + index);
							glBindTexture(GL_TEXTURE_2D, *val);
							glUniform1i(location, index);
							index++;
						}
						else if constexpr (std::is_same_v<T, Vector4>)
						{
							glUniform4fv(location, 1, reinterpret_cast<const float*>(&val));
						}
						else if  constexpr (std::is_same_v<T, Matrix4*>)
						{
							batchOffsets = val;
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
						else if constexpr (std::is_same_v<T, std::vector<GLuint*>>)
						{
							Mesh* myMesh = n->GetMesh();
							glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), index);
							for (int i = 0; i < myMesh->GetSubMeshCount(); ++i)
							{
								glActiveTexture(GL_TEXTURE0 + index);
								glBindTexture(GL_TEXTURE_2D, *val[i]);
								myMesh->DrawSubMesh(i);
							}
							index++;
						}
						else if constexpr (std::is_same_v<T, Material::WorldValue>)
						{
							SetWorldValues(&renderFlag, &faceCulling, &tessFalg, &index, &batchDraws, val, location);
						}
						}, value);
				}
			}
		}

		if (faceCulling == false)
				glDisable(GL_CULL_FACE);

		if (batchOffsets == NULL) 
		{
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, model.values);

			if (renderFlag)
				SetShaderLight(*light);

			if (tessFalg)
				n->Draw(*this, GL_PATCHES);
			else
				n->Draw(*this);
		}
		else 
		{
			for (int i = 0; i < batchDraws; i++) 
			{
				Matrix4 model = batchOffsets[i];
				glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, model.values);
				n->Draw(*this);
			}
		}
		
		if (faceCulling == false)
			glEnable(GL_CULL_FACE);
	}
}

void Renderer::SetTextureParam(int* index, GLint location, GLuint val)
{
	glUniform1i(location, *index);
	glActiveTexture(GL_TEXTURE0 + *index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, val);
	*index += 1;
}


void Renderer::SetWorldValues(bool* renderFlag, bool* faceCulling, bool* tessFalg, int* index, int* batchDraws, Material::WorldValue val, GLint location)
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
		case Material::BatchSize50:
			*batchDraws = 50;
			break;
		case Material::BatchSize100:
			*batchDraws = 100;
			break;
		case Material::BatchSize300:
			*batchDraws = 300;
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


void Renderer::DrawParticles()
{		
	Shader* particleShader = particleManager.GetShader();
	GLuint* particleTexture = particleManager.GetTexture();

	BindShader(particleShader);
	SetProjectionMatrix();
	camera->BuildViewMatrix();
	UpdateShaderMatrices();

	glUniform3fv(glGetUniformLocation(particleShader->GetProgram(), "cameraPosition"), 1, (float*)&lastCameraPos);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_CULL_FACE);
	particleManager.Draw(*this);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
}

void Renderer::DrawNodes()
{
	DrawOpaque();
	DrawTransparent();
}

void Renderer::DrawSkybox() {

	glDepthMask(GL_FALSE);
	BindShader(skyboxShader);
	UpdateShaderMatrices();
	glUniform1f(glGetUniformLocation(skyboxShader->GetProgram(), "temperature"), temperature);

	skyQuad->Draw();
	glDepthMask(GL_TRUE);
}



void Renderer::DrawDepthNodes(Shader* shader)
{
	for (const auto& i : nodeList) {
		DrawNodeWithFallBack(i, shader);
	}
}

void Renderer::DrawTransparent()
{
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::DrawOpaque()
{
	SetProjectionMatrix();
	viewMatrix = camera->BuildViewMatrix();
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
}



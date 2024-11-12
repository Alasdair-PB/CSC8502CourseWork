#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include "Material.h"
#include <vector>

class SceneNode
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	Material* GetMaterial() { return material; }
	void SetMaterial(Material m) { material = &m; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() const { return texture; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	void SetShader(Shader* shader) { this->shader = shader; this->material->SetShader(shader); }
	Shader* GetShader() const { return shader; }

	void AddChild(SceneNode* s);

	virtual void Update(float dt, Vector3 cameraPos);
	virtual void Draw(const OGLRenderer& r, GLuint type = NULL);

	std::vector < SceneNode* >::const_iterator GetChildIteratorStart() {
		return children.begin();
	}

	std::vector < SceneNode* >::const_iterator GetChildIteratorEnd() {
		return children.end();
	}

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }
	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b)
	{
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}	
	
	bool reflect;


protected:
	SceneNode* parent;
	Mesh* mesh;

	Shader* shader;	
	Vector4 colour;
	GLuint texture;
	Material* material;

	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;

	std::vector <SceneNode*> children;

	float distanceFromCamera;
	float boundingRadius;
};

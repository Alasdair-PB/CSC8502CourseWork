#include "SceneNode.h"

SceneNode::SceneNode(Mesh* mesh, Vector4 colour) {
	this->mesh = mesh;
	this->colour = colour;
	this->material = new Material(shader);
	parent = NULL;
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texture = 0;
	modelScale = Vector3(1, 1, 1);
}

SceneNode::~SceneNode(void)
{
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
	delete material;
}

void SceneNode::AddChild(SceneNode* s)
{
	children.push_back(s);
	s->parent = this;

}


void SceneNode::Draw(const OGLRenderer& r, GLuint type)
{
	if (mesh)
		mesh->Draw(type);
}

void SceneNode::Update(float dt, Vector3 cameraPos) 
{

	if (parent)
		worldTransform = parent->worldTransform * transform;
	else
		worldTransform = transform;

	for (vector <SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(dt, cameraPos);
	}
}


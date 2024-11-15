/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Wrapper around OpenGL primitives, geometry and related
OGL functions.

There's a couple of extra functions in here that you didn't get in the tutorial
series, to draw debug normals and tangents.


-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OGLRenderer.h"
#include <vector>
#include <string>

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
enum MeshBuffer {
	VERTEX_BUFFER,
	COLOUR_BUFFER,
	TEXTURE_BUFFER,
	NORMAL_BUFFER,
	TANGENT_BUFFER,

	WEIGHTVALUE_BUFFER,		//new this year, weight values of vertices
	WEIGHTINDEX_BUFFER,	//new this year, indices of weights

	INDEX_BUFFER,

	MAX_BUFFER
};

class Mesh {
public:
	struct SubMesh {
		int start;
		int count;
	};

	static Mesh* GenerateQuad();
	static Mesh* GenerateTriangle();

	Mesh(void);
	~Mesh(void);

	void Draw(GLuint type = NULL);
	void DrawSubMesh(int i);

	static Mesh* LoadFromMeshFile(const std::string& name);

	unsigned int GetTriCount() const {
		int primCount = indices ? numIndices : numVertices;
		return primCount / 3;
	}

	unsigned int GetJointCount() const {
		return (unsigned int)jointNames.size();
	}


	bool RayMeshIntersect(Vector3 origin, Vector3 direction, Vector3* hitPosition, Vector3* normal, float& outDistance);
	bool RayTriangleIntersect(Vector3 origin, Vector3 direction, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& t);
	int GetIndexForJoint(const std::string& name) const;
	int GetParentForJoint(const std::string& name) const;
	int GetParentForJoint(int i) const;
	void InstancedDraw(int count);
	const Matrix4* GetBindPose() const {
		return bindPose;
	}

	const Matrix4* GetInverseBindPose() const {
		return inverseBindPose;
	}

	int	GetSubMeshCount() const {
		return (int)meshLayers.size();
	}

	bool GetSubMesh(int i, const SubMesh* s) const;
	bool GetSubMesh(const std::string& name, const SubMesh* s) const;

protected:
	void	BufferData();
	GLuint numIndices;
	unsigned int* indices;

	GLuint	arrayObject;
	GLuint	bufferObject[MAX_BUFFER];
	GLuint	numVertices;

	GLuint	type;

	Vector3* vertices;
	Vector4* colours;
	Vector2* textureCoords;
	Vector3* normals;
	Vector4* tangents;

	Vector4* weights;
	int* weightIndices;

	Matrix4* bindPose;
	Matrix4* inverseBindPose;

	std::vector<std::string>	jointNames;
	std::vector<int>			jointParents;
	std::vector< SubMesh>		meshLayers;
	std::vector<std::string>	layerNames;


	void GenerateTangents();
	Vector4 GenerateTangent(int a, int b, int c);
	void GenerateNormals();
	bool GetVertexIndicesForTri(unsigned int i, unsigned int& a, unsigned int& b, unsigned int& c) const;
};


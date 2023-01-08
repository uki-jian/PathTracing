#pragma once

#include "super.h"
#include "mesh.h"
#include "ray.h"
#include "aabb.h"
#include "intersect.h"

using namespace std;
using namespace glm;

struct faceInfo {
	vec3 color;
	vector<vec3> vertices;
};

struct OctreeNode {
	AABB aabb;
	int layer;
	vector<int> faces;	//只有叶子节点才存储面索引
	OctreeNode* father;

	/* 2 3
	   0 1
	   ---
	   6 7
	   4 5
	*/
	// up down left right near far
#define uln 0
#define urn 1
#define ulf 2
#define urf 3
#define dln 4
#define drn 5
#define dlf 6
#define drf 7
	OctreeNode* sons[8];
};

class Octree {
public:
	int width;
	int height;
	int maxDepth; //八叉树的深度：根节点=第0层，叶子节点=第depth层
	int epsilon; //八叉树叶子节点的精度,现在没设置好
	int n_faces = 0;
	OctreeNode* oct_root;
	Mesh* mesh_p;

	Octree();
	~Octree();

	void initOctree(Mesh& mesh);
	bool is_leaf_node(OctreeNode* op);
	OctreeNode* initOctreeNode(int layer, float xl, float xr, float yl, float yr, float zl, float zr, OctreeNode* father);
	void addFaces(Mesh& mesh);
	void addFace_per_node(AABB& bb, int faceIdx, OctreeNode* op);
	//bool findFace(OctreeNode* op, int faceIdx);
	bool ifIntersectRay(OctreeNode* op, Ray& ray, Intersect_Primitive& result);
	void releaseNode(OctreeNode* op);
};


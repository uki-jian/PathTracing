#include "octree.h"

Octree::Octree() {};

void Octree::initOctree(Mesh& mesh) {
	clock_t t1 = clock();

	mesh_p = &mesh;
	AABB& aabb = mesh.full_aabb;
	width = aabb.maxx;
	height = aabb.maxy;
	maxDepth = 6;
	epsilon = 0;	//
	n_faces = mesh.faces.size();
	
	oct_root = initOctreeNode(0, aabb.minx, aabb.maxx, aabb.miny, aabb.maxy, aabb.minz, aabb.maxz, NULL);

	addFaces(mesh);

	clock_t t2 = clock();
	cout << "An octree has been built, using: " << t2 - t1 << " ms" << endl;
	cout << n_faces << " faces have been added!" << endl;
}
Octree::~Octree() {
	releaseNode(oct_root);
}

void Octree::releaseNode(OctreeNode* op) {
	if (!op)return;
	for (int i = 0; i < 8; i++)releaseNode(op->sons[i]);
	delete op;
}

//如果是叶子节点，就返回true
bool Octree::is_leaf_node(OctreeNode* op) {
	for (int i = 0; i < 8; i++) {
		if (op->sons[i] != NULL)return false;
	}
	return true;
}
//初始化八叉树节点
OctreeNode* Octree::initOctreeNode(int layer, float xl, float xr, float yl, float yr, float zl, float zr, OctreeNode* father) {

	if (xl >= xr || yl >= yr)return NULL;

	OctreeNode* op = new OctreeNode;
	op->layer = layer;
	op->aabb.minx = xl;
	op->aabb.maxx = xr;
	op->aabb.miny = yl;
	op->aabb.maxy = yr;
	op->aabb.minz = zl;
	op->aabb.maxz = zr;
	op->father = father;
	if (layer >= maxDepth) {
		for (int i = 0; i < 8; i++) {
			op->sons[i] = NULL;
		}
		return op;
	}

	float xmid = (xl + xr) / 2;
	float ymid = (yl + yr) / 2;
	float zmid = (zl + zr) / 2;

	op->sons[uln] = initOctreeNode(layer + 1, xl, xmid, yl, ymid, zmid, zr, op);
	op->sons[urn] = initOctreeNode(layer + 1, xmid, xr, yl, ymid, zmid, zr, op);
	op->sons[ulf] = initOctreeNode(layer + 1, xl, xmid, yl, ymid, zl, zmid, op);
	op->sons[urf] = initOctreeNode(layer + 1, xmid, xr, yl, ymid, zl, zmid, op);
	op->sons[dln] = initOctreeNode(layer + 1, xl, xmid, ymid, yr, zmid, zr, op);
	op->sons[drn] = initOctreeNode(layer + 1, xmid, xr, ymid, yr, zmid, zr, op);
	op->sons[dlf] = initOctreeNode(layer + 1, xl, xmid, ymid, yr, zl, zmid, op);
	op->sons[drf] = initOctreeNode(layer + 1, xmid, xr, ymid, yr, zl, zmid, op);

	return op;
}

void Octree::addFaces(Mesh& mesh) {
	for (Face& face : mesh.faces) {
		addFace_per_node(face.aabb, face.idx, oct_root);
	}
}
void Octree::addFace_per_node(AABB& aabb, int faceIdx, OctreeNode* op) {
	//face完全在这个node之外，直接return
	if (op == NULL)return;
	if (op->aabb.minx > aabb.maxx || op->aabb.maxx < aabb.minx)return;
	if (op->aabb.miny > aabb.maxy || op->aabb.maxy < aabb.miny)return;
	if (op->aabb.minz > aabb.maxz || op->aabb.maxz < aabb.minz)return;

	if(is_leaf_node(op))
		op->faces.push_back(faceIdx);
	else for (int i = 0; i < 8; i++) {
		addFace_per_node(aabb, faceIdx, op->sons[i]);
	}
}
//bool Octree::findFace(OctreeNode* op, int faceIdx) {
//	if (!op)return false;
//	if (op->faces[faceIdx] == 1)return true;
//	else return false;
//}
bool Octree::ifIntersectRay(OctreeNode* op, Ray& ray, Intersect_Primitive& result) {
	if (!op)return false;

	bool is_inter = false;
	if (op->aabb.intersect(ray)) {
		if (is_leaf_node(op)) {
			for (int i = 0; i < op->faces.size(); i++) {
				vec3 intersection;	//交点
				Face& face = mesh_p->faces[op->faces[i]];
				if (face.intersect(ray, intersection)) {
					float dist = glm::distance(intersection, ray.origin);
					if (dist < result.distance) {
						result.distance = dist;
						result.material = face.material;
						result.normal = face.normal;
						result.point = intersection;
						result.getReflectance(face);
					}
					if (!is_inter) {
						is_inter = true;
						INTERSECTED++;
					}
				}
				
			}
		}
		else {
			for (int i = 0; i < 8; i++) {
				is_inter |= ifIntersectRay(op->sons[i], ray, result);	//有一个儿子交就算交
			}
		}
	}
	return is_inter;
}
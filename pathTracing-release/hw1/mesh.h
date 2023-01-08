#pragma once

#include "super.h"
#include "aabb.h"
#include "ray.h"
#include "texture.h"

using namespace std;
using namespace glm;

struct Vertex {
	vec3 position;
};

struct Material {
	string name;
	vec3 Kd = COLOR_BLACK, Ks = COLOR_BLACK;
	float Ns = 0, Ni = 0;
	bool have_KdTexture = false;
	bool have_KsTexture = false;
	Texture KdTexture;
	Texture KsTexture;
	vec3 radiance = vec3(0, 0, 0);

	bool isemission() {		//是否是发光体？
#define FABS_EPSILON 0.001
		if (fabs(radiance.r) < FABS_EPSILON && fabs(radiance.g) < FABS_EPSILON  && fabs(radiance.b) < FABS_EPSILON) return false;
		else return true;
	}
};

// Face must be a triangle
struct Face {
	vector<int> vertexIdx;
	vector<int> textureIdx;
	vector<int> normalIdx;
	AABB aabb;
	int mtlIdx;
	int idx;
	//int maxYVertexIdx = 0;
	//int minYVertexIdx = 0;
	//默认顶点、法线、纹理的坐标应该是一致的
	//vector<int> textureIdx;
	//vector<int> normalIdx;
	//Parameter parameter; //平面方程a,b,c,d; (a,b,c)==normal

	vec3 origin; //vertex0
	vec3 dx, dy;
	vec2 uv;	//材质坐标
	vec2 uv_dx, uv_dy;

	vec3 points[3];
	vec3 normal;
	vec3 central;
	Material* material;
	bool intersect(Ray& ray, vec3& point);
	vec3 getRandPoint();
	vec2 getToocords(vec3& pos);
};



class Mesh{
public:
	AABB full_aabb;
	vector<Vertex> vertices;
	vector<vec3> normals;
	vector<vec2> textures;
	vector<Face> faces;
	vector<Material> materials;
	vector<Face*> lights;	

	Mesh() {};
	~Mesh() {};

	void init(const string& path);
	void faceProcess();

private:
	void loadOBJ(const string& path);
	void loadMTL(const string& path);

	vec3 randFaceColor();
	bool findOrAddMaterial(string& name, int& mtlIdx);
};


#pragma once
#include "super.h"
#include "ray.h"
#include "mesh.h"
#include "octree.h"
#include "intersect.h"
#include "utils.h"
#include "camera.h"

using namespace std;
using namespace glm;

class RayTracer {
public:
	int rayNum;		//每个像素点取样的光线
	int maxDepth;	//最大反射次数
	int lightSamples; //光线取样次数
	int randerIteration; //渲染的轮数，每一轮在屏幕上打印出结果
	float survivalIdx;  //每次trace能存活下来的概率
	vec3 ambientILM = vec3(0.05, 0.03, 0.02);
	//vec3 ambientILM = vec3(0, 0, 0);
	Camera camera;
	Mesh mesh;
	Octree octree;

	vec3** colorCache;	//存放各像素颜色

	void init();
	void render();
	vec3 getRays(double x, double y);
	Ray getRay(double x, double y);
	vec3 trace(Ray& ray, int depth);
	Ray MCSelect(Ray& ray, Intersect_Primitive& primitive);
	bool roulette(double survivalProbablility);
	vec3 renderDirectL(Ray& ray, Intersect_Primitive& primitive);
	static double getFresnelIndex(double ni, double nt, double cosTheta);
	static vec3 getRefractD(const vec3 &i, const vec3 &n, float eta);
	static vec3 getReflectD(const vec3 &i, const vec3 &n);
	vec3 importanceSampling(vec3& up, double n=-1);
	void writeImg(vector<vec3>& results);
	vec3 restrain021(vec3& origin);
	bool isDirectLight(int depth);
};

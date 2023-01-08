#pragma once
#include "super.h"
#include "mesh.h"

using namespace glm;

//��ray�ཻ����Ԫ��Ϣ
struct Intersect_Primitive {
	float distance = 99999; //��Դ�ľ���
	vec3 point;
	vec3 normal;
	Material* material;
	vec3 kd, ks;
	void getReflectance(Face& face);
	vec3 auxiliaryILM(Ray& ray);
};

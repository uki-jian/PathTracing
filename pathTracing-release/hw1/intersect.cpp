#include "intersect.h"

using namespace glm;

void Intersect_Primitive::getReflectance(Face& face) {
	if (face.material->name == string("WoodFloor")) {
		int stop = 1;
	}
	vec2 pos = face.getToocords(point);
	if (material->have_KdTexture) {
		kd = material->KdTexture.getColor(pos);
	}
	else kd = material->Kd;
	if (face.material->name == string("WoodFloor")) {
		int stop = 1;
	}
	if (material->have_KsTexture) {
		ks = material->KsTexture.getColor(pos);
	}
	else ks = material->Ks;
}

vec3 Intersect_Primitive::auxiliaryILM(Ray& ray) {
	if (ray.type == __DIFFUSE)return kd;
	else if (ray.type == __SPECULAR)return ks * dot(normalize(ray.direction), normalize(normal));
	else if (ray.type == __TRANSMISSION)return COLOR_WHITE;
	else return COLOR_BLACK;
}
#pragma once
#include "super.h"
#include "utils.h"

using namespace std;
using namespace glm;

class Texture {
public:
	Texture() {};
	~Texture() {};
	Texture(string filename);
	vec3 getColor(const vec2& uvCoord);
	vec3 biLinear(float x, float y);
private:
	FREE_IMAGE_FORMAT fifmt; //文件格式
	FIBITMAP* image;
	unsigned int height = -1;
	unsigned int width = -1;
};

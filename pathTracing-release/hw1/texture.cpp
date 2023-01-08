#include "texture.h"
using namespace std;
using namespace glm;

Texture::Texture(string filename)
{
	fifmt = FreeImage_GetFIFFromFilename(filename.c_str());
	if (fifmt == -1)
	{
		cout << "ERROR::unsupported texture type : " + filename << endl;
		image = NULL;
		return;
	}
	FIBITMAP *dib = FreeImage_Load(fifmt, filename.c_str(), 0);
	if (dib == NULL) {
		cout << "ERROR::Texture image not found!" << endl;
		exit(1);
	}
	dib = FreeImage_ConvertTo24Bits(dib);
	image = FreeImage_ConvertTo24Bits(FreeImage_Load(fifmt, filename.c_str()));
	width = FreeImage_GetWidth(image);
	height = FreeImage_GetHeight(image);
}

vec3 Texture::getColor(const vec2& uvCoord)
{
	if (image == NULL) return COLOR_BLACK;
	float x = uvCoord.x, y = uvCoord.y;
	while (x < 0)x++;
	while (y < 0)y++;
	while (x >= 1)x--;
	while (y >= 1)y--;
		
	vec3 rgb = biLinear(x, y);

	//逆向gamma校正
	return fifmt == FIF_JPEG ? ImageProcess::gammaCorrection(rgb, true) : rgb;

}

//双线性插值
vec3 Texture::biLinear(float x, float y) {
	assert(x >= 0 && x < 1 && y >= 0 && y < 1);
	float u = x * width, v = y * height;
	float dx = u - int(u);
	float dy = v - int(v);

	RGBQUAD T00, T10, T01, T11;
	FreeImage_GetPixelColor(image, int(u), int(v), &T00);
	FreeImage_GetPixelColor(image, int(u)+1, int(v), &T10);
	FreeImage_GetPixelColor(image, int(u), int(v)+1, &T01);
	FreeImage_GetPixelColor(image, int(u)+1, int(v)+1, &T11);

	vec3 t00 = vec3(T00.rgbRed / 255.0, T00.rgbGreen / 255.0, T00.rgbBlue / 255.0),
		t10 = vec3(T10.rgbRed / 255.0, T10.rgbGreen / 255.0, T10.rgbBlue / 255.0),
		t01 = vec3(T01.rgbRed / 255.0, T01.rgbGreen / 255.0, T01.rgbBlue / 255.0),
		t11 = vec3(T11.rgbRed / 255.0, T11.rgbGreen / 255.0, T11.rgbBlue / 255.0);

	vec3 txy = (1 - dy) * ((1 - dx) * t00 + dx * t10) + dy * ((1 - dx) * t01 + dx * t11);
	return txy;
}
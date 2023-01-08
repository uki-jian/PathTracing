#pragma once

#include "include/GLFW/glfw3.h"
#include "include/glut/glut.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/tinyxml/tinyxml.h"
#include "include/tinyxml/tinystr.h"
#include "include/FreeImage/FreeImage.h"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <time.h>
#include <random>
#include <omp.h>

#define RANDOM_SEED 1

#define COLOR_BLACK vec3(0, 0, 0)
#define COLOR_WHITE vec3(1, 1, 1)
#define NULL_DIRECTION vec3(0, 0, 0)

extern int INTERSECTED;

#define D_EPS 0.001
static const double PI = acos(-1.0);
static const double GAMMA = 2.2;

inline double rand0to1() { return (double)rand() / RAND_MAX; }

#define COLOR_ENHANCE 1.f



//#define __DEBUG		//debug模式下,降低分辨率和采样数
#define GAUSSIAN	//开启高斯滤波

#ifdef __DEBUG
	#define RAYNUM 1
	#define MAXDEPTH 10
	#define LIGHTSAMPLES 1
	#define SURVIVALIDX 0.8
#else
	#define RAYNUM 1
	#define MAXDEPTH 30
	#define LIGHTSAMPLES 1
	#define SURVIVALIDX 0.8
#endif


//#define CORNELL_BOX
#define BEDROOM
//#define VEACH_MIS
//#define THRONE_ROOM
//#define MY_CB

#define MODEL_PATH "../model/"
#define RESULT_PATH "../result/"




#ifdef CORNELL_BOX
	#define DEBUG_WIDTH 256
	#define DEBUG_HEIGHT 256
	#define MODEL_NAME "cornell-box"
#endif


#ifdef BEDROOM
	#define DEBUG_WIDTH 512
	#define DEBUG_HEIGHT 288
	#define MODEL_NAME "bedroom"
#endif


#ifdef VEACH_MIS
	#define DEBUG_WIDTH 120
	#define DEBUG_HEIGHT 90
	#define MODEL_NAME "veach-mis"
#endif

#ifdef THRONE_ROOM
#define DEBUG_WIDTH 100
#define DEBUG_HEIGHT 100
#define MODEL_NAME "throneroom"
#endif

#ifdef MY_CB
#define DEBUG_WIDTH 500
#define DEBUG_HEIGHT 500
#define MODEL_NAME "mycb"
#endif
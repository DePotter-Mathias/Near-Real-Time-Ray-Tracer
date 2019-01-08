#pragma once
//#include "ShaderFunctions.h"
#include "TTF_Wrapper.h"
#include "Manager.h"

#include "Sphere.h"
#include "Plane.h"

#include <SDL.h>

#include <vector>
#include <string>

//number of cores
extern const unsigned int Cores = std::thread::hardware_concurrency();
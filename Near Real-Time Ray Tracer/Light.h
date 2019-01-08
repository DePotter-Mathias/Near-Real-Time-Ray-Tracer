#pragma once
#include "Vec.h"

struct Light
{
	Vec pos;
	Color color;
	float radius;
	Light(Vec pos, float radius, Color color) :
		pos(pos),
		radius(radius),
		color(color)
	{
		color.Normalize();
	}
};
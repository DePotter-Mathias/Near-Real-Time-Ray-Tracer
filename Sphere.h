#pragma once
#include "Entity.h"


struct Sphere : public Entity
{
	double radius;
	Sphere( const Vec& position, double radius, const Material& material ) : radius( radius )
	{
		mat = material;
		pos = position;
	}

	double Intersect( const Vec& orig, const Vec& ray );

	Vec GetNormalAtPoint( const Vec& hit );

	void GetUVCoordinates( double& U, double& V, const double scale, const Vec& hit );
};

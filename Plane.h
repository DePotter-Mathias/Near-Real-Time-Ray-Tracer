#pragma once
#include "Entity.h"

struct Plane : public Entity
{
	Vec norm;
	float offset;

	Plane( const Vec& normal, float offset, const Material& material ) :
		offset( offset ),
		norm(normal.Normalized())
	{
		mat = material;
	}

	double Intersect( const Vec& orig, const Vec& ray );

	Vec GetNormalAtPoint( const Vec& hit );

	void GetUVCoordinates( double& U, double& V, const double scale, const Vec& hit );
};
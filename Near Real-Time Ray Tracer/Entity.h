#pragma once
#include "Material.h"

struct Entity
{
	Vec pos;
	Material mat;

	Entity() {}
	Entity( Vec pos, Material mat ) :pos( pos ), mat( mat ) {}

	virtual double Intersect( const Vec& orig, const Vec& ray ) = 0;
	virtual Vec GetNormalAtPoint( const Vec& hit ) = 0;
	virtual void GetUVCoordinates( double& U, double& V, const double scale, const Vec& hit ) = 0;
};
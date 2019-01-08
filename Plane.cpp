#include "Plane.h"

double Plane::Intersect( const Vec& orig, const Vec& ray )
{
	const double c = ray * norm;
	const double t = -(orig * norm + offset);
	return t / c;
}

Vec Plane::GetNormalAtPoint( const Vec & hit )
{
	return norm;
}

void Plane::GetUVCoordinates( double& U, double& V, const double scale, const Vec& hit )
{
	Vec u;
	u = Vec( 1, 0, 0 ) % norm;
	if (u.LengthSqr() == 0) u = Vec( 0, 1, 0 );
	const Vec v = norm % u;
	double ud = u * hit;
	double vd = v * hit;
	U = ( ud < 0 ? 1 : 0 ) + fmod( ud / scale, 1.0 );
	V = ( vd < 0 ? 1 : 0 ) + fmod( vd / scale, 1.0 );
}
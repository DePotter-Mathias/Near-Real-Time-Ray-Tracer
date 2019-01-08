#include "Sphere.h"

double Sphere::Intersect( const Vec& orig, const Vec& ray )
{
	const Vec toSphere = pos - orig;

	double dot = toSphere * ray;
	if (dot < 0) return -1;

	double dd = toSphere.LengthSqr() - dot * dot - radius;
	if (dd > 0 ) return -1;

	return dot - sqrt14(abs(dd));
}

Vec Sphere::GetNormalAtPoint( const Vec& hit )
{
	return  ( hit - pos ).Normalized();
}

void Sphere::GetUVCoordinates( double& U, double& V, const double scale, const Vec& hit )
{
	Vec n = ( hit - pos ).Normalized();

	double ud = atan2( n.x, n.z ) / 3.14159256 + 0.5;
	double vd = acos( n.y * 0.5 + 0.5 );

	U = ( ud < 0 ? 1 : 0 ) + fmod( ud / scale, 1.0 );
	V = ( vd < 0 ? 1 : 0 ) + fmod( vd / scale, 1.0 );
}
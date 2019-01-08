#include "Camera.h"
#include<vector>

std::vector<Vec> Camera::rays = std::vector<Vec>();

Vec Rotate( const Vec & P, const double( &matrix )[9] )
{
	return{matrix[0] * P.x + matrix[1] * P.y + matrix[2] * P.z,
		matrix[3] * P.x + matrix[4] * P.y + matrix[5] * P.z,
		matrix[6] * P.x + matrix[7] * P.y + matrix[8] * P.z};
}

// M3 by reference, in-out
void MatrixMutiply( const double M1[9], const double M2[9], double( &M3 )[9] )
{
	for (size_t i = 0; i < 9; i += 3)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			M3[i + j] += M1[i] * M2[j];
			M3[i + j] += M1[i + 1] * M2[3 + j];
			M3[i + j] += M1[i + 2] * M2[6 + j];
		}
	}
}

void Camera::Rotate( double DeltaX, double DeltaY )
{
	xRot = Clamp( xRot + DeltaX, xRotMax, xRotMin );
	yRot += DeltaY;

	//roll - along z axis - works
	/*	double ry[9] = {
	cos( yRot ),	-sin( yRot ),	0,
	sin( yRot ),	cos( yRot ),	0,
	0,				0,				1};	*/

	//yaw - along y axis - works
	double ry[9] = {
		cos( yRot ),	0,	sin( yRot ),
		0,				1,	0,
		-sin( yRot ),	0,	cos( yRot )};

	//pitch - along x axis - works
	double rx[9] = {
		1,	0,				0,
		0,	cos( xRot ),	-sin( xRot ),
		0,	sin( xRot ),	cos( xRot )};

	EmptyRotationMatrix();
	MatrixMutiply( ry, rx, rotMatrix );

	//:: global Rotate function
	dir = ::Rotate( {0, 0, 1}, ry );
}

void Camera::ResetRotationMatrix()
{
	//clear
	EmptyRotationMatrix();
	//reset
	rotMatrix[0] = 1;
	rotMatrix[4] = 1;
	rotMatrix[8] = 1;
}


void Camera::EmptyRotationMatrix()
{
	for (size_t i = 0; i < 9; i++) 
		rotMatrix[i] = 0;
}

void Camera::Moved()
{
	timeNotMoved = 0;
}

//generates rays on first use, and returns pointer to either the rays or newly generated when w and h are not 0
std::vector<Vec>* Camera::GetScreenRays( double fov, unsigned width, unsigned height, double aspectRatio )
{
	if (rays.size() > 0)
	{
		return &rays;
	}
	else if (width == 0 || height == 0)
	{
		return nullptr;
	}

	rays.resize( width * height );
	auto t = tan( fov / 2 * 3.14159265 / 180.0 );

	for (unsigned i = 0; i < width * height; i++)
	{
		rays[i] = Vec(
			( 2 * ( ( i % width + 0.5 ) / width ) - 1 ) * t,
			( 1 - 2 * ( ( i / height + 0.5 ) / height ) ) * t * aspectRatio,
			1 ).Normalized();
	}
	return &rays;

}


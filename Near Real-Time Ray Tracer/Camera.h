#pragma once
#include<vector>
#include "Vec.h"

struct Camera
{
private:
	static std::vector<Vec> rays;

public:
	Vec pos, dir;

	unsigned width = 1, height = 1;
	double aspectRatio = 1;
	double fov = 1;
	double focalPoint = 4;
	double xRot = 0;
	double yRot = 0;
	double xRotMax;
	double xRotMin;
	double timeNotMoved = 0;
	double rotMatrix[9] = { 1,0,0,0,1,0,0,0,1 }; //Identity Matrix Default
	bool enableDof = false;

	Camera(const Vec& pos, const Vec& dir, unsigned w, unsigned h, double fov, double xRotMax = 80, double xRotMin = -80) :
		pos(pos),
		dir(dir.Normalized()),
		aspectRatio(w / h),
		width(w),
		height(h),
		fov(fov),
		xRotMax(xRotMax* (3.14159265 / 180.0)),
		xRotMin(xRotMin* (3.14159265 / 180.0))
	{
		Camera::GetScreenRays(fov, w, h, aspectRatio);
		Rotate(atan2(dir.x, dir.z), dir.y);
	}

	~Camera(){}

	void Rotate(double DeltaX, double DeltaY);
	void ResetRotationMatrix();
	void EmptyRotationMatrix();
	void Moved();
	static std::vector<Vec>* GetScreenRays(double fov = 0, unsigned width = 0, unsigned height = 0, double aspectRatio = 0);
};

Vec Rotate(const Vec & P, const double(&matrix)[9]);

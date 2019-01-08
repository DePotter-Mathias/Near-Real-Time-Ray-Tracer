#pragma once
#include <algorithm>
#include <math.h>

//taken from https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi
double inline /*__declspec /*(naked)*/ __fastcall sqrt14(double n)
{
	return sqrt(n);
	/*__asm
	{
		fld qword ptr[esp + 4]
		fsqrt
		ret 8
	};*/
}

template<typename  T>
inline T Clamp(T val, T max = 1, T min = 0) { return std::max(min, std::min(val, max)); }

struct Vec
{
	double x = 0, y = 0, z = 0;
	Vec() {}
	Vec(double x, double y, double z) :
		x(x),
		y(y),
		z(z)
	{
	}

	//Vec( const Vec &p ) : x( p.x ), y( p.y ), z( p.z ) {}

	//const fucntions

	inline double Length()					const { return sqrt14(x * x + y * y + z * z); }
	inline double LengthSqr()				const { return x * x + y * y + z * z; }
	inline Vec operator + (const Vec &p)	const { return{ x + p.x, y + p.y, z + p.z }; }
	inline Vec operator + (double c)		const { return{ x + c, y + c, z + c }; }
	inline Vec operator - (const Vec &p)	const { return{ x - p.x, y - p.y, z - p.z }; }
	inline Vec operator - (double c)		const { return{ x - c, y - c, z - c }; }
	inline Vec operator - ()				const { return (*this) * -1; }
	inline Vec operator * (double c)		const { return{ x*c, y*c, z*c }; }
	inline double operator * (const Vec &p)	const { return x * p.x + y * p.y + z * p.z; }
	inline Vec operator / (double c)		const { return{ x / c, y / c, z / c }; }

	inline void operator += (const Vec &p) { x += p.x; y += p.y; z += p.z; }
	inline void operator += (double p) { x += p; y += p; z += p; }
	inline void operator -= (const Vec &p) { x -= p.x; y -= p.y; z -= p.z; }
	inline void operator -= (double p) { x -= p; y -= p; z -= p; }
	inline void operator *= (double c) { x *= c; y *= c; z *= c; }
	inline void operator *= (Vec c) { x *= c.x; y *= c.y; z *= c.z; }
	inline void operator /= (double c) { (*this) *= 1 / c; }

	//custom operator overload cross
	inline Vec operator%(const Vec & p) const { return{ y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x }; }

	Vec Inverse() const { return{ 1 / x , 1 / y, 1 / z }; }

	Vec Saturate(double max = 1, double min = 0) { return{ Clamp(x, max, min), Clamp(y, max, min), Clamp(z, max, min) }; }

	Vec SaturateTop(double max = 1) { return{ std::min(x, max), std::min(y, max), std::min(z, max) }; }

	inline void Normalize() { *this /= Length(); }

	inline Vec Normalized() const { return *this / Length(); }

	inline Vec ComponentMultiply(const Vec &p) { x *= p.x; y *= p.y; z *= p.z; return *this; };

	inline Vec Reflect(const Vec &normal)const { return *this - (normal * (2 * (*this * normal))); };
};

typedef Vec Color;
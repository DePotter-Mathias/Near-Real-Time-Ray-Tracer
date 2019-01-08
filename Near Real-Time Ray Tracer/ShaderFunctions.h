#pragma once

#include "Light.h"
#include "Material.h"
#include "Manager.h"

#define lerp(a,b,l) (a*l+b*(1-l))
#define REN_DIST_MAX 27 //DBL_MAX-1
#define REN_DIST_MIN 0//DBL_MIN

const int halfInt = 32767 / 2;
unsigned g_seed = 33;

inline int fastrand()//0-32767
{
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed >> 16) & 0x7FFF;
}
inline double fastrandDouble()
{
	return fastrand() / 32767.0;
}



#define randVecUnsigned Vec(fastrandDouble(),fastrandDouble(),fastrandDouble())
#define randVecSigned (randVecUnsigned * 2 - 1)

//by martin ankerl
//taken from https://martin.ankerl.com/2007/10/04/optimized-pow-approximation-for-java-and-c-c/
inline double fastPow(double a, double b)
{
	union { double d; int x[2]; } u = { a };
	u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
	u.x[0] = 0;
	return u.d;
}

inline double schlick(const double refractiveIndex, const Vec& norm, const Vec& ray)
{
	double r = (1 - refractiveIndex) / (1 + refractiveIndex);
	r *= r;
	return r + (1 - r) * fastPow(1 - std::max((-norm)*ray, 0.0), 5);
}

inline Color BlinnPhong(const Color &diffuseColor, const Material& mat, const Vec& orig, const Vec& intersect, const Vec& normal, const Light& light)
{
	const Vec iterectToLight = (light.pos - intersect).Normalized();
	const double dotNL = normal * iterectToLight;

	Color result;

	if (dotNL > 0)
	{
		Color shade = diffuseColor;
		shade.ComponentMultiply(light.color);

		const Vec intersectToEye = (orig - intersect).Normalized();

		const Vec hV = (iterectToLight + intersectToEye).Normalized();
		const double specular = Clamp(fastPow(normal * hV, mat.shininess))* mat.specular;
		result = ((shade *  mat.diffuse * dotNL) + light.color * specular * dotNL).SaturateTop();
	}
	return result;
}

// returns nearest object that intersects vector "ray"
inline double ClosestIntersectDistance(const Vec& from, const Vec& ray, const std::vector<Entity*>& sceneEntities, Entity * & closestEntity)
{
	double currentSmallestFound = DBL_MAX;
	for (Entity* entity : sceneEntities)
	{
		const double distance = entity->Intersect(from, ray);
		bool hit = distance > REN_DIST_MIN && distance < REN_DIST_MAX && distance < currentSmallestFound;
		if (hit)
		{
			currentSmallestFound = distance;
			closestEntity = entity;
		}
	}
	return currentSmallestFound;
}

inline const Vec NormalMapping(const Vec& normal, Vec normalmap, const double normalIntensity)
{
	Vec t = abs(abs(normal.y) - 1.0) < 1e-5 ? Vec(1, 0, 0) : Vec(0, 1, 0) % normal;
	Vec b = normal % t;

	const double local[9] =
	{
		b.x,b.y,b.z,
		t.x,t.y,t.z,
		normal.x,normal.y,normal.z,
	};

	normalmap = Rotate(normalmap * 2 - 1, local);
	return lerp(normal, normalmap, normalIntensity).Saturate();
}

inline Color RecursiveShading(const Vec& from, const Vec& ray, const Scene& scene, unsigned depth, unsigned shadowSamples)
{
	//depth reached
	if (depth < 1) return scene.GetSkyColor();
	--depth;

	//test for object along ray
	Entity* closestEntity = nullptr;
	const double intersectDistance = ClosestIntersectDistance(from, ray, scene.GetEntities(), closestEntity);

	//no object found
	if (closestEntity == nullptr)	return scene.GetSkyColor();

	//set a random vec once; slow fucntion
	const Vec randVec = randVecSigned;
	const Vec hitPoint = (ray * intersectDistance + from);
	//normal at the point
	Vec normalHitPoint = closestEntity->GetNormalAtPoint(hitPoint);

	//if it has a normal map set, use that to set normal
	if (closestEntity->mat.texture.normSet)
	{
		double u, v, s = closestEntity->mat.texture.scale;
		closestEntity->GetUVCoordinates(u, v, s, hitPoint);

		const Vec& normalMap = GetPixel(closestEntity->mat.texture.normal, u, v);
		normalHitPoint = NormalMapping(normalHitPoint, normalMap, closestEntity->mat.texture.normalIntensity);
	}
	normalHitPoint += randVec * closestEntity->mat.diffuse * closestEntity->mat.diffuse * 0.3;
	normalHitPoint.Normalize();

	const Vec offsetWithNormal = hitPoint + normalHitPoint * (1e-4 * intersectDistance);

	const double schlicked = schlick(closestEntity->mat.refractiveIndex, normalHitPoint, ray);

	//transparency
	Color transparency;
	if (closestEntity->mat.transparency > 1e-4)//if transp at all
	{
		const auto offsetWithNegNormal = hitPoint + ray * 1e-4;

		transparency = RecursiveShading(
			offsetWithNegNormal,
			lerp(ray, -normalHitPoint, schlicked).Normalized(),
			scene,
			depth,
			shadowSamples);
		transparency.SaturateTop();
		transparency *= closestEntity->mat.transparency;
	}

	//Reflection
	Color reflectivity;
	const double reflectivityWithSchlick = std::min(schlicked * Clamp(closestEntity->mat.reflectivity * 5) + closestEntity->mat.reflectivity, 1.0);

	reflectivity = RecursiveShading(offsetWithNormal,
		ray.Reflect((normalHitPoint + randVec * closestEntity->mat.diffuse).Normalized()),
		scene,
		depth,
		shadowSamples).SaturateTop();

	//Diffuse
	Color diffuse;
	if (closestEntity->mat.diffuse > 0)
	{
		Color diffuseColor;
		//if it has a texture set, use that as diffuse color
		double u, v, s = closestEntity->mat.texture.scale;
		if (closestEntity->mat.texture.texSet)
		{
			closestEntity->GetUVCoordinates(u, v, s, hitPoint);
			diffuseColor = GetPixel(closestEntity->mat.texture.texture, u, v);
		}
		else
		{
			diffuseColor = closestEntity->mat.color;
		}

		for (const Light& l : scene.GetLights())
		{
			diffuse = (diffuse + BlinnPhong(diffuseColor, closestEntity->mat, from, offsetWithNormal, normalHitPoint, l)).Saturate();
		}
		diffuse *= closestEntity->mat.diffuse * (1 - closestEntity->mat.transparency);
	}

	//Shadows
	Color ShadowColoring;
	//all light
	for (const Light& light : scene.GetLights()) ShadowColoring += light.color;

	const auto& sceneEntities = scene.GetEntities();

	for (const Light& light : scene.GetLights())
	{
		Vec pointToLight = (light.pos - hitPoint).Normalized();

		Entity * closestEntitylight = nullptr;

		for (size_t i = 0; i < shadowSamples; ++i)
		{
			//ray to light hit check
			ClosestIntersectDistance(offsetWithNormal, (pointToLight + randVec / 15 * light.radius).Normalized(), sceneEntities, closestEntitylight);

			//subtract light form total scene light when blocked
			if (closestEntitylight != nullptr)
				ShadowColoring -= light.color / shadowSamples - closestEntitylight->mat.transparency * closestEntitylight->mat.diffuse;
		}
	}

	//final blending
	Color pointColor = diffuse + transparency + reflectivity * reflectivityWithSchlick;
	//shadow light color 
	pointColor *= ShadowColoring;
	//add in reflectivity in shadows
	return ((pointColor + reflectivity * 0.3)*1.3).SaturateTop()+0.02;
}

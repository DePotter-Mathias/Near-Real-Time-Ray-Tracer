#pragma once
#include "SDL_image.h"
#include "Vec.h"
#include <map>

//Refractive Indeces stored without decimal point
enum  MaterialsType
{
	Air = 100028,
	Water = 133300,
	Acryl = 149200,
	Plastic = 158123,
	Glass = 152220,
	Diamond = 240000
};

struct Texture
{
	SDL_Surface *texture;
	SDL_Surface *normal;
	double scale = 1, normalIntensity = 1;

	bool texSet = false, normSet = false;

	Texture() {}

	//Texture Loader
	Texture( const char* textureLocation, const char* normalLocation = nullptr, double scale = 1, double normalIntensity = 1 )
		:scale( scale ), normalIntensity( normalIntensity )
	{
		if (( textureLocation != NULL ) && ( textureLocation[0] == '\0' ))
		{
			return;
		}
		texture = IMG_Load( textureLocation );
		texSet = texture != nullptr ? true : false;
		if (( normalLocation != NULL ) && ( normalLocation[0] == '\0' ))
		{
			return;
		}
		normal = IMG_Load( normalLocation );
		normSet = normal != nullptr ? true : false;
	}
};

inline Color GetPixel( SDL_Surface *surface, double u, double v )
{
	//SDL_LockSurface( surface );
	int bpp = surface->format->BytesPerPixel;

	int index = ( (int) ( u * surface->w ) ) * bpp + ( (int) ( v * surface->h ) ) * surface->pitch;
	Uint8* p = (Uint8*) surface->pixels + index;
	Uint32 color = 0;
	//SDL_UnlockSurface( surface );
	
	switch (bpp)
	{
		case 1:
			color = *p;
			break;
		case 2:
			color = *(Uint16 *) p;
			break;
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				color = p[0] << 16 | p[1] << 8 | p[2];
			else
				color = p[0] | p[1] << 8 | p[2] << 16;
			break;
		case 4:
			color = *(Uint32 *) p;
			break;
	}

	Uint8 r, g, b;
	SDL_GetRGB( color, surface->format, &r, &g, &b );
	return{b / 255.0, g / 255.0, r / 255.0};
}

struct Material
{
	Color color;
	double reflectivity, shininess, diffuse, specular, refractiveIndex, transparency, reflectiveDiffuser;
	Texture texture;
	//more properties...	

	/**
	@param color Sets the color, get normalized internally
	@param diffuse ]0-1[ scalar on diffuse
	@param transparency ]0-1[ 0 is opaque, 1 is invisible
	@param shininess ]0-inf[ sets the power of specular highlight
	@param specular ]0-1[ scalar on specular highlight
	@param reflectivity ]0-1[ 0 is no reflections, 1 is only reflections
	@param materialsType 6-digit int, use  MaterialsType enum presets
	or refractive index with 5 decimals without comma, no negative refractive indeces allowed
	(one number before decimal is assumed)
	*/
	Material( Color color = Color( 1, 0, 1 ),
		double diffuse = 1,
		double transparency = 0,
		double shininess = 100,
		double specular = 1,
		double  reflectivity = 0,
		bool enableSampledShadows = false,
		MaterialsType materialsType = Plastic )
		:
		color( color.Normalized() ),
		diffuse( Clamp( diffuse ) ),
		shininess( std::max( shininess, 0.0 ) ),
		transparency( Clamp( transparency ) ),
		specular( Clamp( specular ) ),
		reflectivity( Clamp( reflectivity ) ),
		refractiveIndex( std::max( ( (unsigned) materialsType ) / 100000.0, 0.0 ) )
	{
	}

	Material( Texture texture,
		double diffuse = 1,
		double transparency = 0,
		double shininess = 100,
		double specular = 1,
		double  reflectivity = 0,
		bool enableSampledShadows = false,
		MaterialsType materialsType = Plastic )
		:
		texture( texture ),
		shininess( std::max( shininess, 0.0 ) ),
		specular( Clamp( specular ) ),
		refractiveIndex( std::max( ( (unsigned) materialsType ) / 100000.0, 0.0 ) )
	{
		this->diffuse = Clamp(diffuse);
		this->transparency = Clamp(transparency);
		this->reflectivity = Clamp(reflectivity);
	}
};
#pragma once
#include "SDL_ttf.h"
#include <string>

class TTF_Wrapper
{
	private:
	SDL_Color textColor, backgroundColor;
	float spaceBetweenText, offsetX, offsetY;
	TTF_Font *font;
	SDL_Renderer* renderer;
	unsigned messageCount = 0;

	public:
	TTF_Wrapper( SDL_Renderer* renderer, const char * fontLocation, unsigned  pointSize = 1, float spaceBetweenText = 10,
		float offsetX = 10,
		float offsetY = 10,
		SDL_Color textColor = {255,255,255,255},
		SDL_Color backgroundColor = {0,0,0,255} ) :
		backgroundColor( backgroundColor ),
		textColor( textColor ),
		offsetY( offsetY ),
		offsetX( offsetX ),
		renderer( renderer ),
		spaceBetweenText( spaceBetweenText )
	{
		TTF_Init();
		font = TTF_OpenFont( fontLocation, pointSize );

	}
	void BackToTop();
	~TTF_Wrapper()
	{
		TTF_CloseFont( font );
		font = nullptr;
	};

	template <class T>
	void DrawInfo( T toDraw )
	{
		DrawInfo( to_std::string( toDraw ) );
	}

	void DrawInfo( const char* toDraw )
	{
		DrawInfo( std::string( toDraw ) );
	}

	void DrawInfo( std::string toDraw );
};
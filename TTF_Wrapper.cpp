#include "TTF_Wrapper.h"

void TTF_Wrapper::BackToTop()
{
	messageCount = 0;
}

void TTF_Wrapper::DrawInfo( std::string toDraw )
{
	if (toDraw.length() == 0)
	{
		++messageCount;
		return;
	}
	SDL_Surface* textSurface = TTF_RenderText_Shaded( font, toDraw.c_str(), textColor, backgroundColor );
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
	SDL_Rect renderQuad = {(int)offsetX, (int) (offsetY + spaceBetweenText * (float)messageCount), textSurface->w, textSurface->h};
	SDL_RenderCopy( renderer, textTexture, NULL, &renderQuad );
	SDL_DestroyTexture( textTexture );
	SDL_FreeSurface( textSurface );
	++messageCount;
}
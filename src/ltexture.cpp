#include "ltexture.h"
#include <iostream>

LTexture::LTexture(SDL_Renderer *gRenderer) : renderer(gRenderer) {
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture() {
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path) {
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if (loadedSurface == NULL) 	{
		std::cout << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
		return false;
	}	else 	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
		//Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL) {
			std::cout << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
			return false;
		} else {
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	//mTexture = std::make_unique<SDL_Texture>(newTexture);
	mTexture = newTexture;
	return true;
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(this->font, textureText.c_str(), textColor);
	SDL_Texture *tempTexture;
	if (textSurface == NULL) 	{
		std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
		return false;
	} else {
		//Create texture from surface pixels
    tempTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
		if (tempTexture == NULL) {
			std::cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
			return false;
		} else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	//mTexture = std::make_unique<SDL_Texture>(tempTexture);
	mTexture = tempTexture;
	//Return success
	return true;
}

void LTexture::setTextFromInt(int val) {
	SDL_Color textColor = {0, 0, 0};
	const char *chWeight = std::to_string(val).c_str();
	if (!this->loadFromRenderedText(chWeight, textColor)) {
		std::cout << "failed to load from rendered text" << std::endl;
	}
}

void LTexture::free() {
	//Free texture if it exists
	SDL_DestroyTexture(mTexture);
	mWidth = 0;
	mHeight = 0;
	//mTexture.reset();
	mTexture = NULL;
}

void LTexture::setFont(TTF_Font *gFont) {
	this->font = gFont;
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue );
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending );
}

void LTexture::setAlpha(Uint8 alpha) {
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip ) {
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	//Set clip rendering dimensions
	if (clip != NULL) 	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

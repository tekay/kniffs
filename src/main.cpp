/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

#include "ltexture.h"
/*#include "lbutton.h"
#include "loader.h"*/
#include "singleplayer.h"
#include <iostream>
#include <memory>

//Screen dimension constants
const int SCREEN_WIDTH = 430;
const int SCREEN_HEIGHT = 900;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

struct sdl_deleter
{
  void operator()(SDL_Window *p) const { SDL_DestroyWindow(p); }
  void operator()(SDL_Renderer *p) const { SDL_DestroyRenderer(p); }
  void operator()(SDL_Texture *p) const { SDL_DestroyTexture(p); }
	void operator()(TTF_Font *p) const { TTF_CloseFont(p); }
};

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
//std::shared_ptr<SDL_Renderer> gRenderer;
SDL_Renderer *gRenderer;

//Globally used font
//std::shared_ptr<TTF_Font> gFont;
TTF_Font *gFont;

std::unique_ptr<Singleplayer> game;


bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		success = false;
	}	else {
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))	{
			std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
		}

		//Create window
		gWindow = SDL_CreateWindow( "Kniffs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if (gWindow == NULL) {
			std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
			success = false;
		}	else {
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if (gRenderer == NULL) {
				std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
				success = false;
			}	else {
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
					success = false;
				}
				//Initialize SDL_ttf
				if (TTF_Init() == -1) {
					std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia() {
	//Loading success flag
	bool success = true;
	gFont = TTF_OpenFont("resources/fonts/Roboto-Bold.ttf", 28);
	if (gFont == NULL) {
		std::cout << "Failed to load font, error: " << TTF_GetError() << std::endl;
		success = false;
	}

	//Nothing to load
	return success;
}

void close() {
	game.reset();

	//Destroy window
	TTF_CloseFont(gFont);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	gFont = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[]) {
	//Start up SDL and create window
	if (!init()) {
		std::cout << "Failed to initialize!" << std::endl;
	} else {
		//Load media
		if (!loadMedia()) {
			std::cout << "Failed to load media!" << std::endl;
		} else {
			game = std::make_unique<Singleplayer>(gRenderer, gFont);
			//Main loop flag
			bool quit = false;
			//Event handler
			SDL_Event e;

			//While application is running
			while (!quit) {
				//Handle events on queue
				while (SDL_PollEvent( &e ) != 0) {
					//User requests quit
					if (e.type == SDL_QUIT) {
						quit = true;
					}
					if (game->handleKeyEvents(e) == 1) {
						quit = true;
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				game->render();

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}
	//Free resources and close SDL
	close();

	return 0;
}

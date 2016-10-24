#ifndef LOADER_H__
#define LOADER_H__

#include "ltexture.h"
#include "ball.h"

class Loader {
	public:
		const int LOADER_MOVE_WIDTH = 50;
		const int LOADER_WIDTH = 80;
		const int LOADER_HEIGHT = 80;
		const int LOADER_Y_POS = 160;
		Loader(SDL_Renderer *gRenderer);
		~Loader();

		void move(int direction);
		void render();
		void giveBall(Ball *ball);
		Ball* dropBall();
		int getCurrentCol();

	private:
		int xPos;
		Ball *ball;
		LTexture *texture;
};
#endif LOADER_H__

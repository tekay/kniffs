#ifndef LOADER_H__
#define LOADER_H__

#include "ltexture.h"
#include "ball.h"
#include "standardball.h"

class Loader {
	public:
		const int LOADER_MOVE_WIDTH = 50;
		const int LOADER_WIDTH = 80;
		const int LOADER_HEIGHT = 80;
		const int LOADER_Y_POS = 160;
		const int MAX_COL = 8;
		Loader(SDL_Renderer *gRenderer);
		~Loader();

		void move(int direction);
		void render();
		void giveBall(std::shared_ptr<Ball> ball);
		std::shared_ptr<Ball> dropBall();
		unsigned int getCurrentCol();

	private:
		unsigned int col;
		unsigned int xPos;
		std::shared_ptr<Ball> ball;
		std::unique_ptr<LTexture> texture;
};
#endif // LOADER_H__

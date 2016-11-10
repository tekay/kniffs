#ifndef BALLSOURCE_H__
#define BALLSOURCE_H__

#include "ball.h"
#include "standardball.h"
#include <memory>

class BallSource {
	public:
		BallSource(SDL_Renderer *gRenderer, TTF_Font *gFont, int leftOffset, int topOffset);
		~BallSource();
		std::shared_ptr<Ball> getBallAt(int col, int level);
		void render();

	private:
		// functions
		std::shared_ptr<Ball> makeNewBall(int level);

		// constants
		static const int NEW_BALLS_ROW_COUNT = 2;
		static const int COL_COUNT = 8;

		// important
		std::shared_ptr<Ball> newBalls[COL_COUNT][NEW_BALLS_ROW_COUNT];

		// graphics
		int leftOffset;
		int topOffset;

		SDL_Renderer *renderer;
		TTF_Font *font;
};

#endif

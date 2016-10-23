#ifndef FIELD_H__
#define FIELD_H__

#include "ltexture.h"
#include "loader.h"
#include "ball.h"

class Field {
	public:
		Field(SDL_Renderer *gRenderer, TTF_Font *gFont);
		~Field();

		int handleKeyEvents(SDL_Event &e);
		void render();
	private:
		static const int ROW_LENGTH = 8;
		static const int COL_COUNT = 8;
		static const int NEW_BALLS_ROWS = 2;
		static const int STACK_HEIGHT = 10;
		static const int LEFT_OFFSET = 15;
		Ball* makeNewBall();
		Ball* getBallFromNew(int col);
		// returns whether balls has place
		int dropBall(Ball *ball, int col);
		void checkField(int row, int col);
		bool checkForStacking(int row, int col);
		void stackBalls(int row, int col);
		bool checkPosForDestroying(int row, int col);
		bool checkAndDestroy();
		void destroyBalls(int row, int col, int color);
		void collapseField();
		void collapseStack(int row, int col);

		void setBallToPos(int row, int col);
		void setWeightForWeightTexture(int col);

		Loader *loader;
		LTexture *weightTextures[COL_COUNT];
		int weights[COL_COUNT];

		// new ball row
		Ball *newBalls[NEW_BALLS_ROWS][ROW_LENGTH];
		// ball stacks
		Ball *stackedBalls[STACK_HEIGHT][ROW_LENGTH];

		int level;
		int ballsPlaced;
		int points;

		SDL_Renderer *renderer;
		TTF_Font *font;
};

#endif FIELD_H__

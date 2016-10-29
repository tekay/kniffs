#ifndef FIELD_H__
#define FIELD_H__

#include "ltexture.h"
#include "loader.h"
#include "ball.h"
#include "slot.h"
#include "scale.h"

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
		static const int TOP_HEIGHT = 60;
		static const int BALLS_PLACED_TEXTURE_X_POS = 120;
		static const int BALLS_PLACED_TEXTURE_Y_POS = 0;
		static const int BALLS_PLACED_TEXT_TEXTURE_X_POS = 10;
		static const int BALLS_PLACED_TEXT_TEXTURE_Y_POS = 0;
		static const int POINTS_TEXTURE_X_POS = 325;
		static const int POINTS_TEXTURE_Y_POS = 0;
		static const int POINTS_TEXT_TEXTURE_X_POS = 200;
		static const int POINTS_TEXT_TEXTURE_Y_POS = 0;

		Ball* makeNewBall();
		Ball* getBallFromNew(int col);
		// returns whether balls has place
		int dropBall(Ball *ball, int col);
		void checkField(int row, int col);
		bool checkForStacking(int row, int col);
		void stackBalls(int row, int col);
		bool checkPosForDestroying(int row, int col);
		bool checkAndDestroy();
		void destroyBalls(int row, int col, int color, int *count, int *sumWeight);
		void collapseField();
		void collapseStack(int row, int col);

		void calculateAndAddPoints(int count, int weight);

		void setBallToPos(int row, int col);
		void setTextFromIntForTexture(LTexture *texture, int val);

		LTexture *weightTextures[COL_COUNT];
		LTexture *ballsPlacedTexture;
		LTexture *ballsPlacedTextTexture;
		LTexture *pointsTexture;
		LTexture *pointsTextTexture;

		// the scales
		Scale *scales[COL_COUNT / 2];

		// new ball row
		Slot *newBalls[NEW_BALLS_ROWS][ROW_LENGTH];
		// ball stacks
		Slot *stackedBalls[STACK_HEIGHT][ROW_LENGTH];

		Loader *loader;
		int weights[COL_COUNT];
		int level;
		int ballsPlaced;
		int points;

		SDL_Renderer *renderer;
		TTF_Font *font;
};

#endif FIELD_H__

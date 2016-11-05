#ifndef FIELD_H__
#define FIELD_H__

#include "ltexture.h"
#include "loader.h"
#include "ball.h"
#include "scale.h"
#include "event.h"
#include <memory>

class Field {
	public:
		Field(SDL_Renderer *gRenderer, TTF_Font *gFont, int leftOffset, int topOffset);
		~Field();

		int handleKeyEvents(SDL_Event &e);
		void render();
		bool dropBallAt(std::shared_ptr<Ball> ball, int col);
		void check();

	private:
		static const int ROW_LENGTH = 8;
		static const int COL_COUNT = 8;
		static const int SCALE_COUNT = 4;
		static const int STACK_HEIGHT = 10;
		static const int LEFT_OFFSET = 15;
		static const int TOP_HEIGHT = 60;

		// returns whether balls has place
		std::array<int, 2> getScaleAndColFromCol(int col);
		int handleBallThrowing(std::shared_ptr<Event> event, int startCol);

		std::unique_ptr<Scale> scales[SCALE_COUNT];

		// graphics
		int leftOffset;
		int topOffset;

		SDL_Renderer *renderer;
		TTF_Font *font;
};

#endif // FIELD_H__

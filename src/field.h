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
		Field(SDL_Renderer *gRenderer, TTF_Font *gFont, std::shared_ptr<unsigned int> points, int leftOffset, int topOffset);
		~Field();

		int handleKeyEvents(SDL_Event &e);
		void render();
		bool dropBallAt(std::shared_ptr<Ball> ball, int col);
		bool check();

	private:
		static const int ROW_LENGTH = 8;
		static const int COL_COUNT = 8;
		static const int SCALE_COUNT = 4;
		static const int SCALE_COL_COUNT = 2;
		static const int STACK_HEIGHT = 10;
		static const int LEFT_OFFSET = 15;
		static const int TOP_HEIGHT = 60;

		// returns whether balls has place
		void calculateAndAddPoints(unsigned int sumWeight, unsigned int count);
		bool destroying();
		void destroyCrawler(int col, int row, std::shared_ptr<unsigned int> sumWeight, std::shared_ptr<unsigned int> count);
		void collapseAndStack();
		std::array<int, 2> getScaleAndColFromCol(int col);
		int handleBallThrowing(std::shared_ptr<Event> event);

		std::shared_ptr<unsigned int> points;
		std::unique_ptr<Scale> scales[SCALE_COUNT];

		// graphics
		int leftOffset;
		int topOffset;

		SDL_Renderer *renderer;
		TTF_Font *font;
};

#endif // FIELD_H__

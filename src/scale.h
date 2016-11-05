#ifndef SCALE_H__
#define SCALE_H__

#include "ltexture.h"
#include "ball.h"
#include "event.h"
#include <memory>

class Scale {
	public:
		Scale(SDL_Renderer *gRenderer, TTF_Font *gFont, int leftOffset, int topOffset);
		~Scale();

		std::shared_ptr<Ball> getBallAt(int col, int row);
		std::shared_ptr<Event> dropBallAt(std::shared_ptr<Ball> ball, int col);
		void stacking(int col);

		// graphics
		void render();
	private:
		// constants
		static const int STACK_HEIGHT = 10;
		static const int COL_COUNT = 2;
		static const int SCALE_TEXTURE_WIDTH = 100;
		static const int SCALE_TEXTURE_HEIGHT = 110;
		static const int SCALE_TOP_OFFSET = 500;

		// functions
		bool relocateStacks(int oldStatus);
		bool stackUp(int col, int firstElem);
		void stackDown(int col, int firstElem);
		int firstBallSlot(int col);
		int newStatus();
		std::shared_ptr<Ball> getAndRemoveBallFromTop(int col);
		void setBallToPos(int row, int col);

		// status: 0 = neutral, 1 = left down, 2 = right down;
		int status;
		int weights[COL_COUNT];
		std::shared_ptr<Ball> stacks[COL_COUNT][STACK_HEIGHT];
		int leftOffset;
		int topOffset;

		// graphics
		SDL_Renderer *renderer;
		TTF_Font *font;
		std::unique_ptr<LTexture> texture;
		std::unique_ptr<LTexture> weightTextures[COL_COUNT];
};

#endif

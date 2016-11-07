#ifndef SINGLEPLAYER_H__
#define SINGLEPLAYER_H__

#include <memory>
#include "loader.h"
#include "ballsource.h"
#include "field.h"

class Singleplayer {
	public:
		Singleplayer(SDL_Renderer *gRenderer, TTF_Font *gFont);
		~Singleplayer();

		int handleKeyEvents(SDL_Event &e);

		// graphics
		void render();

	private:
		// functions

		// constants
		static const int TOP_HEIGHT = 50;
		static const int TOP_LEFT_OFFSET = 15;
		static const int FIELD_LEFT_OFFSET = 15;
		static const int FIELD_TOP_OFFSET = 230;
		static const int NEW_BALLS_ROW_COUNT = 2;
		static const int COL_COUNT = 8;
		static const int BALLS_PLACED_TEXTURE_X_POS = 120;
		static const int BALLS_PLACED_TEXTURE_Y_POS = 0;
		static const int BALLS_PLACED_TEXT_TEXTURE_X_POS = 10;
		static const int BALLS_PLACED_TEXT_TEXTURE_Y_POS = 0;
		static const int POINTS_TEXTURE_X_POS = 325;
		static const int POINTS_TEXTURE_Y_POS = 0;
		static const int POINTS_TEXT_TEXTURE_X_POS = 200;
		static const int POINTS_TEXT_TEXTURE_Y_POS = 0;

		// important stuff
		std::unique_ptr<Loader> loader;
		std::unique_ptr<BallSource> ballSource;
		std::unique_ptr<Field> field;
		unsigned int ballsPlaced;
		std::shared_ptr<unsigned int> points;
		unsigned int level;

		// graphics stuff
		SDL_Renderer *renderer;
		TTF_Font *font;
		std::shared_ptr<SDL_Color> fontColor;
		std::unique_ptr<LTexture> ballsPlacedTexture;
		std::unique_ptr<LTexture> ballsPlacedTextTexture;
		std::unique_ptr<LTexture> pointsTexture;
		std::unique_ptr<LTexture> pointsTextTexture;
};

#endif

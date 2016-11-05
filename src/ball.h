#ifndef BALL_H__
#define BALL_H__

#include "ltexture.h"
#include <memory>

class Ball {
	public:
		static const int BALL_WIDTH = 50;
		static const int BALL_HEIGHT = 50;

		Ball(SDL_Renderer *gRenderer, TTF_Font *gFont, int color, int weight);
		~Ball();

		void move();
		void render();
		void setXPos(int x);
		int getXPos();
		void setYPos(int y);
		int getYPos();
		void setPos(int x, int y);
		int getColor();
		void setWeight(int w);
		int getWeight();
		void addWeight(int w);

		// graphical stuff
		void collapse();
		void destroy();

	private:
		int xPos;
		int yPos;
		int weight;
		int color;
		std::unique_ptr<LTexture> texture;
		std::unique_ptr<LTexture> textTexture;
		SDL_Color textColor;
		SDL_Renderer *renderer;
		TTF_Font *font;
};
#endif // BALL_H__

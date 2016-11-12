#ifndef STANDARDBALL_H__
#define STANDARDBALL_H__

#include "ball.h"

class StandardBall : public Ball {
	public:
		StandardBall(SDL_Renderer *gRenderer, TTF_Font *gFont, int color, int weight);
		StandardBall(SDL_Renderer *gRenderer, TTF_Font *gFont);
		~StandardBall();

		virtual bool compare(std::shared_ptr<Ball> other);
		virtual void render();
		virtual void setXPos(int x);
		virtual int getXPos();
		virtual void setYPos(int y);
		virtual int getYPos();
		virtual void setPos(int x, int y);
		virtual int getColor();
		virtual void setWeight(int w);
		virtual int getWeight();
		virtual void addWeight(int w);

		// graphical stuff
		virtual void collapse();
		virtual void dropDown();
		virtual void destroy();

	protected:
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

#endif

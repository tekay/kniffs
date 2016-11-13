#ifndef BALL_H__
#define BALL_H__

#include "ltexture.h"
#include <memory>

class Ball {
	public:
		static const int BALL_WIDTH = 50;
		static const int BALL_HEIGHT = 50;
		static const int COLOR_COUNT = 11;

		enum Specials {
			JOKER = 100
		};

		virtual bool compare(std::shared_ptr<Ball> other) = 0;
		virtual void render() = 0;
		virtual void setXPos(int x) = 0;
		virtual int getXPos() = 0;
		virtual void setYPos(int y) = 0;
		virtual int getYPos() = 0;
		virtual void setPos(int x, int y) = 0;
		virtual void setColor(int color) = 0;
		virtual int getColor() = 0;
		virtual void setWeight(int w) = 0;
		virtual int getWeight() = 0;
		virtual void addWeight(int w) = 0;

		// graphical stuff
		virtual void collapse() = 0;
		virtual void dropDown() = 0;
		virtual void destroy() = 0;
};
#endif // BALL_H__

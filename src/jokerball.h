#ifndef JOKERBALL_H__
#define JOKERBALL_H__

#include "standardball.h"
#include <memory>

class JokerBall : public StandardBall {
	public:
		JokerBall(SDL_Renderer *gRenderer, TTF_Font *gFont);
		~JokerBall();

		virtual bool compare(std::shared_ptr<Ball> other);

		virtual void render();
};

#endif

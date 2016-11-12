#include "jokerball.h"

#include <iostream>

JokerBall::JokerBall(SDL_Renderer *gRenderer, TTF_Font *gFont) : StandardBall(gRenderer, gFont) {
	this->color = Specials::JOKER;
	this->weight = 0;
	this->texture = std::make_unique<LTexture>(gRenderer);
	if (!this->texture->loadFromFile("resources/ball_joker.png")) {
		std::cout << "error loading JokerBall" << std::endl;
		exit(13);
	}
}

JokerBall::~JokerBall() {
	this->texture.reset();
}

bool JokerBall::compare(std::shared_ptr<Ball> other) {
	if (other) {
		return true;
	} else {
		return false;
	}
}

void JokerBall::render() {
	this->texture->render(xPos, yPos);
}

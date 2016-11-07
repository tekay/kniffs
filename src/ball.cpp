#include "ball.h"
#include <iostream>
#include <sstream>

Ball::Ball(SDL_Renderer *gRenderer, TTF_Font *gFont, int color, int weight) : renderer(gRenderer), font(gFont), weight(weight) {
	if (color < COLOR_COUNT) this->color = color;
	else this->color = COLOR_COUNT - 1;
	SDL_Color textColor = {0, 0, 0};
	this->texture = std::make_unique<LTexture>(gRenderer);
	this->textTexture = std::make_unique<LTexture>(gRenderer);
	std::string chColor;
	switch (this->color) {
		case 0:
			chColor = "blue";
			break;
		case 1:
			chColor = "red";
			break;
		case 2:
			chColor = "green";
			break;
		case 3:
			chColor = "orange";
			break;
		case 4:
			chColor = "yellow";
			break;
		case 5:
			chColor = "teal";
			break;
		case 6:
			chColor = "purple";
			break;
		case 7:
			chColor = "darkgreen";
			break;
		case 8:
			chColor = "darkred";
			break;
		case 9:
			chColor = "darkblue";
			break;
		case 10:
			chColor = "grey";
			break;
	}
	std::stringstream ss;
	ss << "resources/ball_" << chColor << ".png";
	if (!this->texture->loadFromFile(ss.str())) {
		std::cout << "error loading ball " << chColor << std::endl;
		exit(13);
	}
	this->textTexture->setFont(gFont);
	textColor = {0, 0, 0};
	std::stringstream ss1;
	ss1 << this->weight;
	const char *chWeight = ss1.str().c_str();
	if (!this->textTexture->loadFromRenderedText(chWeight, textColor)) {
		std::cout << "failed to load from rendered text" << std::endl;
	}
}

Ball::~Ball() {
	this->texture.reset();
	this->textTexture.reset();
}

bool Ball::compare(std::shared_ptr<Ball> other) {
	if (!other) return false;
	return this->color == other->getColor();
}

void Ball::render() {
	this->texture->render(xPos, yPos);
	this->textTexture->render(xPos + 15, yPos + 5);
}

void Ball::setXPos(int x) {
	xPos = x;
}

int Ball::getXPos() {
	return this->xPos;
}

void Ball::setYPos(int y) {
	this->yPos = y;
}

int Ball::getYPos() {
	return this->yPos;
}

void Ball::setPos(int x, int y) {
	xPos = x;
	yPos = y;
}

int Ball::getColor() {
	return this->color;
}

void Ball::setWeight(int w) {
	this->weight = w;
	std::stringstream ss1;
	ss1 << this->weight;
	const char *chWeight = ss1.str().c_str();
	if (!this->textTexture->loadFromRenderedText(chWeight, textColor)) {
		std::cout << "failed to load from rendered text" << std::endl;
	}
}

int Ball::getWeight() {
	return this->weight;
}

void Ball::addWeight(int w) {
	this->weight += w;
	std::stringstream ss1;
	ss1 << this->weight;
	const char *chWeight = ss1.str().c_str();
	if (!this->textTexture->loadFromRenderedText(chWeight, textColor)) {
		std::cout << "failed to load from rendered text" << std::endl;
	}
}

void Ball::collapse() {

}

void Ball::dropDown() {

}

void Ball::destroy() {

}

#include "standardball.h"
#include <iostream>
#include <sstream>

StandardBall::StandardBall(SDL_Renderer *gRenderer, TTF_Font *gFont, int color, int weight) : renderer(gRenderer), font(gFont), weight(weight) {
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
		std::cout << "error loading StandardBall " << chColor << std::endl;
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

StandardBall::StandardBall(SDL_Renderer *gRenderer, TTF_Font *gFont) : renderer(gRenderer), font(gFont) {
	textColor = {0, 0, 0};
}

StandardBall::~StandardBall() {
	this->texture.reset();
	this->textTexture.reset();
}

bool StandardBall::compare(std::shared_ptr<Ball> other) {
	if (!other) return false;
	return this->color == other->getColor();
}

void StandardBall::render() {
	this->texture->render(xPos, yPos);
	this->textTexture->render(xPos + 15, yPos + 5);
}

void StandardBall::setXPos(int x) {
	xPos = x;
}

int StandardBall::getXPos() {
	return this->xPos;
}

void StandardBall::setYPos(int y) {
	this->yPos = y;
}

int StandardBall::getYPos() {
	return this->yPos;
}

void StandardBall::setPos(int x, int y) {
	xPos = x;
	yPos = y;
}

int StandardBall::getColor() {
	return this->color;
}

void StandardBall::setWeight(int w) {
	this->weight = w;
	std::stringstream ss1;
	ss1 << this->weight;
	const char *chWeight = ss1.str().c_str();
	if (!this->textTexture->loadFromRenderedText(chWeight, textColor)) {
		std::cout << "failed to load from rendered text" << std::endl;
	}
}

int StandardBall::getWeight() {
	return this->weight;
}

void StandardBall::addWeight(int w) {
	this->weight += w;
	std::stringstream ss1;
	ss1 << this->weight;
	const char *chWeight = ss1.str().c_str();
	if (!this->textTexture->loadFromRenderedText(chWeight, textColor)) {
		std::cout << "failed to load from rendered text" << std::endl;
	}
}

void StandardBall::collapse() {

}

void StandardBall::dropDown() {

}

void StandardBall::destroy() {

}

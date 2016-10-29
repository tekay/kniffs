#include "ball.h"
#include <sstream>

Ball::Ball(SDL_Renderer *gRenderer, TTF_Font *gFont, int level) {
	int newColor = (rand() % (int)(level));
	int newWeight = (rand() % (int)(level + 1));
	this->font = gFont;
	this->texture = new LTexture(gRenderer);
	this->textTexture = new LTexture(gRenderer);
	this->color = newColor;
	this->weight = newWeight;
	char* chColor;
	switch (newColor) {
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
		default:
			chColor = "grey";
	}
	std::stringstream ss;
	ss << "resources/ball_" << chColor << ".png";
	if (!this->texture->loadFromFile(ss.str())) {
		printf("error loading ball %s", chColor);
		exit(13);
	}
	this->textTexture->setFont(gFont);
	textColor = {0, 0, 0};
	std::stringstream ss1;
	ss1 << this->weight;
	const char *chWeight = ss1.str().c_str();
	if (!this->textTexture->loadFromRenderedText(chWeight, textColor)) {
		printf("failed to load from rendered text\n");
	}
}

Ball::~Ball() {
	delete this->texture;
	delete this->textTexture;
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
	yPos = y;
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
		printf("failed to load from rendered text\n");
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
		printf("failed to load from rendered text\n");
	}
}

void Ball::collapse() {

}

void Ball::destroy() {

}

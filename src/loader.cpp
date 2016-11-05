#include "loader.h"
#include <iostream>

Loader::Loader(SDL_Renderer *gRenderer) {
	this->texture = std::make_unique<LTexture>(gRenderer);
	if (!this->texture->loadFromFile("resources/loader.png")) {
		std::cout << "error loading loader texture" << std::endl;
		exit(13);
	}
	xPos = 0;
	col = 0;
}

Loader::~Loader() {
	this->texture.reset();
}

// direct: 0 = left; 1 = right
void Loader::move(int direction) {
	if (direction == 0) {
		if (this->col > 0) {
			this->xPos -= LOADER_MOVE_WIDTH;
			this->col--;
			if (this->ball) {
				this->ball->setXPos(this->ball->getXPos() - LOADER_MOVE_WIDTH);
			}
		}
	} else if (direction == 1) {
		// TODO: make global consts for Screen width etc.
		if (this->col < MAX_COL - 1) {
			this->xPos += LOADER_MOVE_WIDTH;
			this->col++;
			if (this->ball) {
				this->ball->setXPos(this->ball->getXPos() + LOADER_MOVE_WIDTH);
			}
		}
	}
}

void Loader::render() {
	this->texture->render(xPos, LOADER_Y_POS);
	if (this->ball) this->ball->render();
}

void Loader::giveBall(std::shared_ptr<Ball> ball) {
	//std::cout << "loader has ball w/ color: " << ball->getColor() << std::endl;
	this->ball = ball;
	this->ball->setPos(this->xPos + 15, LOADER_Y_POS + 15);
}

std::shared_ptr<Ball> Loader::dropBall() {
	std::shared_ptr<Ball> retBall = this->ball;
	//this->ball.reset();
	return retBall;
}

unsigned int Loader::getCurrentCol() {
	return col;
}

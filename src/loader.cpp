#include "loader.h"

Loader::Loader(SDL_Renderer *gRenderer) {
	this->texture = new LTexture(gRenderer);
	if (!this->texture->loadFromFile("resources/loader.png")) {
		printf("error loading loader texture\n");
		exit(13);
	}
	xPos = 0;
	this->ball = NULL;
}

Loader::~Loader() {
	delete this->texture;
}

// direct: 0 = left; 1 = right
void Loader::move(int direction) {
	if (direction == 0) {
		if ((xPos - LOADER_MOVE_WIDTH) >= 0) {
			xPos -= LOADER_MOVE_WIDTH;
			if (this->ball != NULL) {
				this->ball->setXPos(this->ball->getXPos() - LOADER_MOVE_WIDTH);
			}
		}
	} else if (direction == 1) {
		// TODO: make global consts for Screen width etc.
		if ((xPos + LOADER_MOVE_WIDTH) <= 350) {
			xPos += LOADER_MOVE_WIDTH;
			if (this->ball != NULL) {
				this->ball->setXPos(this->ball->getXPos() + LOADER_MOVE_WIDTH);
			}
		}
	}
}

void Loader::render() {
	this->texture->render(xPos, LOADER_Y_POS);
	if (this->ball != NULL) this->ball->render();
}

void Loader::giveBall(Ball *ball) {
	//printf("loader has ball w/ color: %d\n", ball->getColor());
	this->ball = ball;
	this->ball->setPos(this->xPos + 15, LOADER_Y_POS + 15);
}

Ball* Loader::dropBall() {
	Ball* retBall = this->ball;
	this->ball = NULL;
	return retBall;
}

int Loader::getCurrentCol() {
	return (xPos / LOADER_MOVE_WIDTH);
}

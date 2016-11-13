#include "ballsource.h"

BallSource::BallSource(SDL_Renderer *gRenderer, TTF_Font *gFont, std::shared_ptr<unsigned int> ballsPlaced, int leftOffset, int topOffset) : renderer(gRenderer), font(gFont), ballsPlaced(ballsPlaced), leftOffset(leftOffset), topOffset(topOffset) {
	int i, j;
	for (i = 0; i < COL_COUNT; i++) {
		for (j = 0; j < NEW_BALLS_ROW_COUNT; j++) {
			newBalls[i][j] = this->makeNewBall(4);
			newBalls[i][j]->setPos(this->leftOffset + i * Ball::BALL_WIDTH, this->topOffset + j * Ball::BALL_HEIGHT);
		}
	}
}

BallSource::~BallSource() {
	int i, j;
	for (i = 0; i < COL_COUNT; i++) {
		for (j = 0; j < NEW_BALLS_ROW_COUNT; j++) {
			newBalls[i][j].reset();
		}
	}
}

std::shared_ptr<Ball> BallSource::getBallAt(int col, int level) {
	std::shared_ptr<Ball> retBall = newBalls[col][1];
	newBalls[col][1] = newBalls[col][0];
	newBalls[col][1]->setYPos(newBalls[col][0]->getYPos() + Ball::BALL_HEIGHT);
	newBalls[col][0] = this->makeNewBall(level);
	newBalls[col][0]->setPos(this->leftOffset + col * Ball::BALL_WIDTH, this->topOffset);
	return retBall;
}

std::shared_ptr<Ball> BallSource::makeNewBall(int level) {
	if (*this->ballsPlaced > 50 && *this->ballsPlaced % 10 == 0) {
		return std::make_shared<JokerBall>(this->renderer);
	} else {
		int newColor = (rand() % (int)(level));
		int newWeight = (rand() % (int)(level + 1));
		return std::make_shared<StandardBall>(this->renderer, this->font, newColor, newWeight);
	}
}

void BallSource::render() {
	int i, j;
	for (i = 0; i < COL_COUNT; i++) {
		for (j = 0; j < NEW_BALLS_ROW_COUNT; j++) {
			newBalls[i][j]->render();
		}
	}
}

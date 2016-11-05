#include "field.h"
#include <iostream>
#include <sstream>

Field::Field(SDL_Renderer *gRenderer, TTF_Font *gFont, int leftOffset, int topOffset) : renderer(gRenderer), font(gFont), leftOffset(leftOffset), topOffset(topOffset) {
	int i;
	for (i = 0; i < SCALE_COUNT; i++) {
		this->scales[i] = std::make_unique<Scale>(this->renderer, this->font, this->leftOffset + i * 100, this->topOffset);
	}
}

Field::~Field() {
	int i;
	for (i = 0; i < SCALE_COUNT; i++) {
		this->scales[i].reset();
	}
}

void Field::render() {
	for (std::unique_ptr<Scale> &s : scales) {
		s->render();
	}
}

bool Field::dropBallAt(std::shared_ptr<Ball> ball, int col) {
	std::array<int, 2> dropPos = this->getScaleAndColFromCol(col);
	//printf("scale: %d, col: %d\n", dropPos[0], dropPos[1]);
	std::shared_ptr<Event> event = this->scales[dropPos[0]]->dropBallAt(ball, dropPos[1]);
	if (event->getType() == Event::LOSS) {
		return false;
	} else if (event->getType() == Event::THROW_BALL) {
		int newDropCol = this->handleBallThrowing(event, col);
		//printf("throw event: ball color: %d, dropCol: %d\n", event->getBall()->getColor(), newDropCol);
		return this->dropBallAt(event->getBall(), newDropCol);
	} else {
		return true;
	}
}

void Field::check() {

}

// private functions
// [0] is the scale, [1] the col in the scale
std::array<int, 2> Field::getScaleAndColFromCol(int col) {
	std::array<int, 2> retArr = {col / 2, col % 2};
	return retArr;
}

int Field::handleBallThrowing(std::shared_ptr<Event> event, int startCol) {
	int newDistance = event->getDistance();
	int dropCol;
	if (event->getDirection() == 1) {
		// ball flies to the left
		startCol++;
		while ((startCol - newDistance) < 0) {
			newDistance -= 8;
		}
		dropCol = startCol - newDistance;
	} else if (event->getDirection() == 2) {
		// ball flies to the right
		startCol--;
		while ((startCol + newDistance) >= COL_COUNT)	{
			newDistance -= 8;
		}
		dropCol = startCol + newDistance;
	}
	return dropCol;
}

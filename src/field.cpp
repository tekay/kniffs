#include "field.h"
#include <iostream>
#include <sstream>

Field::Field(SDL_Renderer *gRenderer, TTF_Font *gFont, std::shared_ptr<unsigned int> points, int leftOffset, int topOffset) : renderer(gRenderer), font(gFont), points(points), leftOffset(leftOffset), topOffset(topOffset) {
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
		// since the startCol is set inside the scale, we need to set the correct startCol for the event
		int correctStartCol = dropPos[0] * SCALE_COL_COUNT + event->getStartCol();
		event->setStartCol(correctStartCol);
		int newDropCol = this->handleBallThrowing(event);
		//printf("throw event: ball color: %d, dropCol: %d\n", event->getBall()->getColor(), newDropCol);
		return this->dropBallAt(event->getBall(), newDropCol);
	} else {
		return true;
	}
}

// returns false if game is lost
bool Field::check() {
	int i;
	// check, if balls are going to be destroyed
	while (this->destroying()) {
		// if balls got destroyed, stacks need to collapse/stack
		//printf("found smth to destroy, collapse stacks!\n");
		this->collapseAndStack();
		// since we destroyed some balls, scales need to be "rechecked", maybe balls will be thrown
		for (i = 0; i < SCALE_COUNT; i++) {
			std::shared_ptr<Event> event = this->scales[i]->adjust();
			if (event->getType() == Event::LOSS) {
				return false;
			} else if (event->getType() == Event::THROW_BALL) {
				// since the startCol is set inside the scale, we need to set the correct startCol for the event
				int correctStartCol = i * SCALE_COL_COUNT + event->getStartCol();
				event->setStartCol(correctStartCol);
				int newDropCol = this->handleBallThrowing(event);
				//printf("throw event: ball color: %d, dropCol: %d\n", event->getBall()->getColor(), newDropCol);
				return this->dropBallAt(event->getBall(), newDropCol);
			}
		}
	}
	return true;
}

// private functions
// returns whether something was destroyed
bool Field::destroying() {
	int i, j;
	bool destroyedSomething = false;
	// testing left and right, so don't check the outer balls
	for (i = 1; i < SCALE_COUNT * SCALE_COL_COUNT - 1; i++) {
		// 0. row can't be in a kniff
		for (j = 1; j < STACK_HEIGHT; j++) {
			std::array<int, 2> currentPos = this->getScaleAndColFromCol(i);
			std::shared_ptr<Ball> currentBall = this->scales[currentPos[0]]->getBallAt(currentPos[1], j);
			if (currentBall) {
				std::array<int, 2> leftPos = this->getScaleAndColFromCol(i - 1);
				std::array<int, 2> rightPos = this->getScaleAndColFromCol(i + 1);
				if (currentBall->compare(this->scales[leftPos[0]]->getBallAt(leftPos[1], j))
						&& currentBall->compare(this->scales[rightPos[0]]->getBallAt(leftPos[1], j))) {
					// found a kniff --> destrooooooooy
					std::shared_ptr<unsigned int> sumWeight = std::make_shared<unsigned int>(0);
					std::shared_ptr<unsigned int> count = std::make_shared<unsigned int>(0);
					this->destroyCrawler(i, j, sumWeight, count);
					this->calculateAndAddPoints(*sumWeight, *count);
					destroyedSomething = true;
				}
			}
		}
	}
	return destroyedSomething;
}

void Field::destroyCrawler(int col, int row, std::shared_ptr<unsigned int> sumWeight, std::shared_ptr<unsigned int> count) {
	//std::cout << "destroying. col: " << col << ", row: " << row << std::endl;
	std::array<int, 2> pos = this->getScaleAndColFromCol(col);
	// temporarily get current Ball
	std::shared_ptr<Ball> cBall = this->scales[pos[0]]->getAndRemoveBallAt(pos[1], row);
	int i;
	// the if (cBall) isn't neccessary when the function is called "right"
	if (cBall) {
		(*count)++;
		(*sumWeight) += cBall->getWeight();
		if (col > 0) {
			// check left
			std::array<int, 2> leftPos = this->getScaleAndColFromCol(col - 1);
			if (cBall->compare(this->scales[leftPos[0]]->getBallAt(leftPos[1], row))) {
				this->destroyCrawler(col - 1, row, sumWeight, count);
			}
		}
		if ((col + 1) < (SCALE_COUNT * SCALE_COL_COUNT)) {
			// check right
			std::array<int, 2> rightPos = this->getScaleAndColFromCol(col + 1);
			if (cBall->compare(this->scales[rightPos[0]]->getBallAt(rightPos[1], row))) {
				this->destroyCrawler(col + 1, row, sumWeight, count);
			}
		}
		if (row > 0) {
			// check "down"
			std::array<int, 2> bottomPos = this->getScaleAndColFromCol(col);
			if (cBall->compare(this->scales[bottomPos[0]]->getBallAt(bottomPos[1], row - 1))) {
				this->destroyCrawler(col, row - 1, sumWeight, count);
			}
		}
		if ((row + 1) < STACK_HEIGHT) {
			// check up
			std::array<int, 2> upPos = this->getScaleAndColFromCol(col);
			if (cBall->compare(this->scales[upPos[0]]->getBallAt(upPos[1], row + 1))) {
				this->destroyCrawler(col, row + 1, sumWeight, count);
			}
		}
		cBall->destroy();
		cBall.reset();
	}
}

void Field::collapseAndStack() {
	int i;
	for (i = 0; i < SCALE_COUNT; i++) {
		this->scales[i]->collapse();
		this->scales[i]->stack();
	}
}

// [0] is the scale, [1] the col in the scale
std::array<int, 2> Field::getScaleAndColFromCol(int col) {
	std::array<int, 2> retArr = {col / 2, col % 2};
	return retArr;
}

int Field::handleBallThrowing(std::shared_ptr<Event> event) {
	int startCol = event->getStartCol();
	int newDistance = event->getDistance();
	int dropCol;
	if (event->getDirection() == 1) {
		// ball flies to the left
		while ((startCol - newDistance) < 0) {
			newDistance -= 8;
		}
		dropCol = startCol - newDistance;
	} else if (event->getDirection() == 2) {
		// ball flies to the right
		while ((startCol + newDistance) >= COL_COUNT)	{
			newDistance -= 8;
		}
		dropCol = startCol + newDistance;
	}
	return dropCol;
}

void Field::calculateAndAddPoints(unsigned int sumWeight, unsigned int count) {
	*this->points = count * sumWeight;
}

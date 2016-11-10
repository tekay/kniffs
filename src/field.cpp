#include "field.h"
#include "logger.h"
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

/**
	Handles the ball after it has left the loader.
	Since the scale can shift and throw a ball, we need to first set the correct start col.
	Then, the target drop col is calculated and the ball is being droped into the correct scale.
	This function is recursive!

	@param ball the ball that is droped onto the scale
	@param col the column based on the field
	@return Whether the game is lost after this ball drop
*/
bool Field::dropBallAt(std::shared_ptr<Ball> ball, int col) {
	std::array<int, 2> dropPos = this->getScaleAndColFromCol(col);
	Logger::info("dropping a ball in column " + std::to_string(col) + " (scale " + std::to_string(dropPos[0]) + " in column: " + std::to_string(dropPos[1]) + ")");
	std::shared_ptr<Event> event = this->scales[dropPos[0]]->dropBallAt(ball, dropPos[1]);
	if (event->getType() == Event::LOSS) {
		Logger::info("game is lost");
		return false;
	} else if (event->getType() == Event::THROW_BALL) {
		// since the startCol is set inside the scale, we need to set the correct startCol for the event
		int correctStartCol = dropPos[0] * SCALE_COL_COUNT + event->getStartCol();
		event->setStartCol(correctStartCol);
		int newDropCol = this->handleBallThrowing(event);
		printf("throw event: ball color: %d, dropCol: %d\n", event->getBall()->getColor(), newDropCol);
		Logger::info("throw event: ball w/ color: " + std::to_string(event->getBall()->getColor()) + ", destination column: " + std::to_string(newDropCol));
		return this->dropBallAt(event->getBall(), newDropCol);
	} else {
		return true;
	}
}

/**
	Handles the validation and chain reactions.
	First, it check's whether something can be destroyed.

	@return whether the game is lost
*/
bool Field::check() {
	int i;
	// check, if balls are going to be destroyed
	while (this->destroying()) {
		// if balls got destroyed, stacks need to collapse/stack
		Logger::info("destroyed some balls, now collapsing the stacks");
		this->collapseAndStack();
		// since we destroyed some balls, scales need to be "rechecked", maybe balls will be thrown
		for (i = 0; i < SCALE_COUNT; i++) {
			std::shared_ptr<Event> event = this->scales[i]->adjust();
			if (event->getType() == Event::LOSS) {
				Logger::info("game is lost");
				return false;
			} else if (event->getType() == Event::THROW_BALL) {
				// since the startCol is set inside the scale, we need to set the correct startCol for the event
				int correctStartCol = i * SCALE_COL_COUNT + event->getStartCol();
				event->setStartCol(correctStartCol);
				int newDropCol = this->handleBallThrowing(event);
				Logger::info("throw event: ball w/ color: " + std::to_string(event->getBall()->getColor()) + ", destination column: " + std::to_string(newDropCol));
				return this->dropBallAt(event->getBall(), newDropCol);
			}
		}
	}
	return true;
}

// private functions

/**
	Checks if three balls with the same color are in a row.
	If a "kniff" is found, it triggeres the destroying.
	Since the field can heavily change, it needs to track wether something got destroyed to clean up the field afterwards.

	@return whether something was destroyed
*/
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

/**
	Crawls from ball to ball with the same color and destroyes them.
	It removes the ball referenced by col and row, adds the weight to the sumWeight and increases the counter.
	Compares nearby slots to the currenct target, if they compare,
	calls the function on this ball to crawl through the field.

	@param col the column of the target
	@param row the row of the target
	@sumWeight the weight of the balls destroyed so far sumed up
	@count the destroy counter
*/
void Field::destroyCrawler(int col, int row, std::shared_ptr<unsigned int> sumWeight, std::shared_ptr<unsigned int> count) {
	Logger::info("destroying ball at col: " + std::to_string(col) + ", row: " + std::to_string(row));
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

/**
	Let's the stacks on the scale collapse (check for "holes").
	See Scales.
*/
void Field::collapseAndStack() {
	int i;
	for (i = 0; i < SCALE_COUNT; i++) {
		this->scales[i]->collapse();
		this->scales[i]->stack();
	}
}

/**
	Calculates, which scale is responsible for the given col.
	Also calculates the column in the scale.

	@param col the column based on the field
	@return the linked scale (ret[0]) and column based on the scale (ret[1])
*/
// [0] is the scale, [1] the col in the scale
std::array<int, 2> Field::getScaleAndColFromCol(int col) {
	std::array<int, 2> retArr = {col / 2, col % 2};
	return retArr;
}

/**
	Calculates the target column for a throw event.

	@param event the throw event
	@return the target drop column based on the field
*/
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
	*this->points += count * sumWeight;
}

#include "scale.h"

Scale::Scale(SDL_Renderer *gRenderer, TTF_Font *gFont, int leftOffset, int topOffset) : renderer(gRenderer), font(gFont), leftOffset(leftOffset), topOffset(topOffset) {
	int i, j;
	this->status = 0;
	this->texture = std::make_unique<LTexture>(gRenderer);
	if (!this->texture->loadFromFile("resources/scales.png")) {
		printf("error loading scale png\n");
		exit(13);
	} else {
    //Set sprite clips
		for (i = 0; i < this->SCALE_STATUS_COUNT; i++) {
			this->spriteClips[i].x = i * 100;
			this->spriteClips[i].y = 0;
			this->spriteClips[i].w = 100;
			this->spriteClips[i].h = 120;
		}
	}
	for (i = 0; i < COL_COUNT; i++) {
		this->weights[i] = 0;
		this->weightTextures[i] = std::make_unique<LTexture>(gRenderer);
		this->weightTextures[i]->setFont(this->font);
		this->weightTextures[i]->setTextFromInt(this->weights[i]);
		for (j = 0; j < STACK_HEIGHT; j++) {
			stacks[i][j] = std::shared_ptr<Ball>();
		}
	}
}

Scale::~Scale() {
	this->texture.reset();
	int i, j;
	for (i = 0; i < COL_COUNT; i++) {
		this->weightTextures[i].reset();
		for (j = 0; j < STACK_HEIGHT; j++) {
			this->stacks[i][j].reset();
		}
	}
}

/**
	Endpoint to get a shared_ptr to the slot.
	Doesn't check whether the slot is occupied by a ball, so it can return a nullptr.

	@param col the column based on the scale
	@param row the row
	@return the slot referenced by col + row. slot can be empty!
*/
std::shared_ptr<Ball> Scale::getBallAt(int col, int row) {
	std::shared_ptr<Ball> retBall = this->stacks[col][row];
	return retBall;
}

/**
	Endpoint to get and remove a shared_ptr to the slot.
	Doesn't check whether the slot is occupied by a ball, so it can return a nullptr.

	@param col the column based on the scale
	@param row the row
	@return the slot referenced by col + row. slot can be empty!
*/
std::shared_ptr<Ball> Scale::getAndRemoveBallAt(int col, int row) {
	std::shared_ptr<Ball> retBall = this->stacks[col][row];
	this->stacks[col][row] = std::shared_ptr<Ball>(nullptr);
	if (retBall) {
		this->weights[col] -= retBall->getWeight();
		this->weightTextures[col]->setTextFromInt(this->weights[col]);
	}
	return retBall;
}

/**
	Handles the ball drop.
	It checks if the ball can be placed.
	Adjusts the scale and checks for stacking.

	@param ball the ball that is droped onto the scale
	@param col the column based on the scales column count
	@return event that has been triggered
*/
std::shared_ptr<Event> Scale::dropBallAt(std::shared_ptr<Ball> ball, int col) {
	if (col >= COL_COUNT || col < 0) {
		// need to make this an exception!
		return std::make_shared<Event>(Event::LOSS);
	}
	int dropRow = STACK_HEIGHT;
	int i;
	for (i = this->firstBallSlot(col); i < STACK_HEIGHT; i++) {
		if (!stacks[col][i]) {
			this->stacks[col][i] = ball;
			//this->stacks[col][i]->setPos(this->leftOffset + col * Ball::BALL_WIDTH, this->topOffset + this->BALL_AREA_HEIGHT -  i * Ball::BALL_HEIGHT);
			this->setBallToPos(col, i);
			dropRow = i;
			break;
		}
	}
	if (dropRow < STACK_HEIGHT) {
		// in this Block, balls aren't neccessarily correct according to status
		this->weights[col] += ball->getWeight();
		this->weightTextures[col]->setTextFromInt(weights[col]);
		this->stacking(col);
		return this->adjust();
	} else {
		return std::make_shared<Event>(Event::LOSS);
	}
}

/**
	Checks whether the scale needs to be shifted.
	When it's shifted, look for a ball to throw and throw it!

	@return event that has been triggered
*/
std::shared_ptr<Event> Scale::adjust() {
	std::shared_ptr<Event> event = std::make_shared<Event>(Event::NOTHING);
	int oldStatus = this->status;
	this->status = this->newStatus();
	//printf("new status: %d\n", this->status);
	// conditions for a flying ball
	if ((oldStatus != this->status) && (this->status != 0)) {
		if (this->status == 1) {
			std::shared_ptr<Ball> maybeBall = this->getAndRemoveBallFromTop(1);
			if (maybeBall) {
				event->setType(Event::THROW_BALL);
				event->setDirection(this->status);
				event->setBall(maybeBall);
				event->setStartCol(1);
				// add weight from ball 'cause it already got removed
				int distance = weights[0] - (weights[1] + maybeBall->getWeight());
				event->setDistance(distance);
			}
		} else if (this->status == 2) {
			std::shared_ptr<Ball> maybeBall = this->getAndRemoveBallFromTop(0);
			if (maybeBall) {
				event->setType(Event::THROW_BALL);
				event->setDirection(this->status);
				event->setBall(maybeBall);
				event->setStartCol(0);
				// add weight from ball 'cause it already got removed
				int distance = weights[1] - (weights[0] + maybeBall->getWeight());
				event->setDistance(distance);
			}
		}
	}
	// relocate balls, check for death!
	if (oldStatus != this->status) {
		int everythingAllRight = this->relocateStacks(oldStatus);
		if (!everythingAllRight) {
			return std::make_shared<Event>(Event::LOSS);
		}
	}
	return event;
}

/**
	Checks for "holes" (destroyed balls) and fills the slot with a ball above this slot.
*/
void Scale::collapse() {
	int i, j, k;
	for (i = 0; i < COL_COUNT; i++) {
		for (j = this->firstBallSlot(i); j < STACK_HEIGHT; j++) {
			if (!this->stacks[i][j] && j < (STACK_HEIGHT - 1)) {
				// possibly hole in the stack. look for a ball to fill it
				for (k = j + 1; k < STACK_HEIGHT; k++) {
					if (this->stacks[i][k]) {
						// found a ball, let's reallocate it
						this->stacks[i][j] = this->stacks[i][k];
						this->stacks[i][k]->dropDown();
						this->setBallToPos(i, j);
						this->stacks[i][k] = std::shared_ptr<Ball>(nullptr);
					}
				}
			}
		}
	}
}

void Scale::stack() {
	int i;
	for (i = 0; i < COL_COUNT; i++) {
		this->stacking(i);
	}
}

void Scale::render() {
	SDL_Rect *clip = &this->spriteClips[this->status];
	this->texture->render(this->leftOffset, this->topOffset + this->SCALE_TOP_OFFSET, clip);
	// render weightTextures
	int i, j;
	for (i = 0; i < COL_COUNT; i++) {
		this->weightTextures[i]->render(this->leftOffset + i * 50, this->topOffset + SCALE_TOP_OFFSET + SCALE_TEXTURE_HEIGHT + 20);
	}
	// render balls
	for (i = 0; i < COL_COUNT; i++) {
		for (j = 0; j < STACK_HEIGHT; j++) {
			if (stacks[i][j]) {
				stacks[i][j]->render();
			}
		}
	}
}

// private functions

/**
	Checks if 5 balls with the same color are laying consecutively on each other.
	If that's the case, add the weight of the top 4 balls to the fifths and remove them from the scale.

	@param col the column based on the scale
*/
void Scale::stacking(int col) {
	if (col >= COL_COUNT || col < 0) {
		return;
	}
	int i;
	int color = -1;
	int sameColor = 0;
	int firstElem = -1;
	// checkForStacking
	for (i = STACK_HEIGHT - 1; i >= firstBallSlot(col); i--) {
		if (stacks[col][i]) {
			if (color == -1) {
				firstElem = i;
				color = stacks[col][i]->getColor();
				sameColor++;
			} else {
				if (stacks[col][i]->getColor() == color) {
					sameColor++;
					if (sameColor > 4) break;
				} else {
					sameColor = 0;
					color = -1;
				}
			}
		}
	}
	// stack!
	if (sameColor > 4) {
		int sumWeight = 0;
		for (i = firstElem; i > (firstElem - 4); i--) {
			sumWeight += stacks[col][i]->getWeight();
			this->stacks[col][i]->collapse();
			this->stacks[col][i].reset();
		}
		stacks[col][firstElem - 4]->addWeight(sumWeight);
	}
}

/**
	This function needs to be called after the scale has shifted (the status has changed).
	It relocates the stacks according to the new status.
	Since a stackUp Operation can kill the player, this is observed.

	@param oldStatus the status prior to the shifting
	@return whether the game is lost
*/
bool Scale::relocateStacks(int oldStatus) {
	//printf("relocateStacks\n");
	//frkn magic. some1 smarter than me needs to look at this.
	bool allRight = true;
	if (oldStatus == 0 && this->status == 1) {
		this->stackDown(0, 1);
		allRight = this->stackUp(1, 1);
	} else if (oldStatus == 0 && this->status == 2) {
		allRight = this->stackUp(0, 1);
		this->stackDown(1, 1);
	} else if (oldStatus == 1 && this->status == 0) {
		allRight = this->stackUp(0, 0);
		this->stackDown(1, 2);
	} else if (oldStatus == 1 && this->status == 2) {
		this->stackDown(1, 2);
		if (this->stackUp(0, 0)) {
			allRight = this->stackUp(0, 1);
			this->stackDown(1, 1);
		} else {
			allRight = false;
		}
	} else if (oldStatus == 2 && this->status == 0) {
		this->stackDown(0, 2);
		allRight = this->stackUp(1, 0);
	} else if (oldStatus == 2 && this->status == 1) {
		this->stackDown(0, 2);
		if (this->stackUp(1, 0)) {
			allRight = this->stackUp(1, 1);
			this->stackDown(0, 1);
		} else {
			allRight = false;
		}
	}
	return allRight;
}

/**
	Puts all balls in the stack one position up.
	When the stack is full, the function returns with a false.

	@param col the column based on the scale
	@param firstElem the row position of the first element to be altered
	@return whether the operation was successfull
*/
bool Scale::stackUp(int col, int firstElem) {
	if (stacks[col][STACK_HEIGHT - 1]) {
		// top place is already occupied --> loss
		return false;
	} else {
		int i;
		// ball that is "in the scale" is at firstBallSlot - 1
		for (i = STACK_HEIGHT - 2; i >= firstElem; i--) {
			if (!this->stacks[col][i]) continue;
			this->stacks[col][i + 1] = stacks[col][i];
			//this->stacks[col][i + 1]->setYPos(this->stacks[col][i + 1]->getYPos() - Ball::BALL_HEIGHT);
			this->setBallToPos(col, i + 1);
			this->stacks[col][i] = std::shared_ptr<Ball>(nullptr);
		}
		return true;
	}
}

/**
	Puts all balls in the stack on position down.

	@param col the column based on the scale
	@param firstElem the row position of the first element to be altered
*/
void Scale::stackDown(int col, int firstElem) {
	int i;
	int firstBallSlot = this->firstBallSlot(col);
	for (i = firstElem - 1; i < STACK_HEIGHT - 2; i++) {
		if (this->stacks[col][i + 1]) {
			this->stacks[col][i] = stacks[col][i + 1];
			//this->stacks[col][i]->setYPos(this->stacks[col][i]->getYPos() + Ball::BALL_HEIGHT);
			this->setBallToPos(col, i);
			this->stacks[col][i + 1] = std::shared_ptr<Ball>(nullptr);
		}
	}
}

/**
	This function calculates the first slot for a ball based on the current status of the scale.

	@param col the column based on the scale
	@return the first slot that can be occupied by a ball
*/
int Scale::firstBallSlot(int col) {
	if (this->status == 0) {
		return 1;
	} else if (this->status == 1) {
		if (col == 0) return 0;
		else return 2;
	} else if (this->status == 2) {
		if (col == 0) return 2;
		else return 0;
	}
}

/**
	Calculates the new status based on the weights.

	@return the new status
*/
int Scale::newStatus() {
	int newStatus;
	if (weights[0] == weights[1]) {
		newStatus = 0;
	} else if (weights[0] < weights[1]) {
		newStatus = 2;
	} else if (weights[0] > weights[1]) {
		newStatus = 1;
	}
	return newStatus;
}

/**
	Removes the top ball from a stack.
	If there is a ball, the weight is substracted.

	@param col the column based on the scale
	@return either a ball or a nullptr
*/
std::shared_ptr<Ball> Scale::getAndRemoveBallFromTop(int col) {
	int i;
	for (i = STACK_HEIGHT - 1; i >= 0; i--) {
		if (this->stacks[col][i]) {
			std::shared_ptr<Ball> retBall = stacks[col][i];
			stacks[col][i] = std::shared_ptr<Ball>(nullptr);
			this->weights[col] -= retBall->getWeight();
			this->weightTextures[col]->setTextFromInt(this->weights[col]);
			return retBall;
		}
	}
	return std::shared_ptr<Ball>(nullptr);
}

/**
	Used to set the correct position for a ball

	@param col the column based on the scale
	@param row the row
*/
void Scale::setBallToPos(int col, int row) {
	this->stacks[col][row]->setPos(this->leftOffset + col * Ball::BALL_WIDTH, this->topOffset + (STACK_HEIGHT - row) * Ball::BALL_HEIGHT);
}

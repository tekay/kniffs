#include "scale.h"

Scale::Scale(SDL_Renderer *gRenderer, TTF_Font *gFont, int leftOffset, int topOffset) : renderer(gRenderer), font(gFont), leftOffset(leftOffset), topOffset(topOffset) {
	this->status = 0;
	this->texture = std::make_unique<LTexture>(gRenderer);
	if (!this->texture->loadFromFile("resources/scale_neutral.png")) {
		printf("error loading scale png\n");
		exit(13);
	}
	int i, j;
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

}

std::shared_ptr<Ball> Scale::getBallAt(int col, int row) {
	return stacks[col][row];
}

std::shared_ptr<Event> Scale::dropBallAt(std::shared_ptr<Ball> ball, int col) {
	std::shared_ptr<Event> event = std::make_shared<Event>(Event::NOTHING);
	if (col >= COL_COUNT || col < 0) {
		event->setType(Event::LOSS);
		return event;
	}
	int dropRow = STACK_HEIGHT;
	int i;
	for (i = this->firstBallSlot(col); i < STACK_HEIGHT; i++) {
		if (!stacks[col][i]) {
			printf("ball in row: %d\n", i);
			this->stacks[col][i] = ball;
			this->stacks[col][i]->setPos(this->leftOffset + col * Ball::BALL_WIDTH, this->topOffset + SCALE_TOP_OFFSET -  i * Ball::BALL_HEIGHT);
			dropRow = i;
			break;
		}
	}
	if (dropRow < STACK_HEIGHT) {
		// in this Block, balls aren't neccessarily correct according to status
		this->weights[col] += ball->getWeight();
		this->weightTextures[col]->setTextFromInt(weights[col]);
		this->stacking(col);
		int oldStatus = this->status;
		this->status = this->newStatus();
		printf("new status: %d\n", this->status);
		// conditions for a flying ball
		if ((oldStatus != this->status) && (this->status != 0)) {
			if (this->status == 1) {
				printf("%s\n", "ball throw to the left");
				std::shared_ptr<Ball> maybeBall = this->getAndRemoveBallFromTop(1);
				if (maybeBall) {
					printf("throw with ball\n");
					event->setType(Event::THROW_BALL);
					event->setDirection(this->status);
					event->setBall(maybeBall);
					int distance = weights[0] - weights[1];
					event->setDistance(distance);
				}
			} else if (this->status == 2) {
				std::shared_ptr<Ball> maybeBall = this->getAndRemoveBallFromTop(0);
				printf("%s\n", "ball throw to the right");
				if (maybeBall) {
					printf("throw with ball\n");
					event->setType(Event::THROW_BALL);
					event->setDirection(this->status);
					event->setBall(maybeBall);
					int distance = weights[1] - weights[0];
					event->setDistance(distance);
				}
			}
		}
		// relocate balls, check for death!
		if (oldStatus != this->status) {
			int everythingAllRight = this->relocateStacks(oldStatus);
			if (!everythingAllRight) {
				event->setType(Event::LOSS);
			}
		}
	} else {
		event->setType(Event::LOSS);
	}
	return event;
}

void Scale::stacking(int col) {
	if (col >= COL_COUNT || col < 0) {
		return;
	}
	int i;
	int color = -1;
	int sameColor = 0;
	int firstElem = 0;
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
					color = -1;
				} else {
					sameColor = 0;
				}
			}
		}
	}
	// stack!
	if (sameColor > 4) {
		int sumWeight = 0;
		for (i = firstElem; i > (firstElem - 4); i++) {
			sumWeight += stacks[col][i]->getWeight();
			stacks[col][i]->collapse();
			stacks[col][i] = std::shared_ptr<Ball>(nullptr);
		}
		stacks[col][firstElem - 4]->addWeight(sumWeight);
	}
}

void Scale::render() {
	this->texture->render(this->leftOffset, this->topOffset + SCALE_TOP_OFFSET);
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
bool Scale::relocateStacks(int oldStatus) {
	printf("relocateStacks\n");
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
			this->stacks[col][i + 1]->setYPos(this->stacks[col][i + 1]->getYPos() - Ball::BALL_HEIGHT);
			this->stacks[col][i] = std::shared_ptr<Ball>(nullptr);
		}
		return true;
	}
}

void Scale::stackDown(int col, int firstElem) {
	int i;
	int firstBallSlot = this->firstBallSlot(col);
	for (i = firstElem - 1; i < STACK_HEIGHT - 2; i++) {
		if (this->stacks[col][i + 1]) {
			this->stacks[col][i] = stacks[col][i + 1];
			this->stacks[col][i]->setYPos(this->stacks[col][i]->getYPos() + Ball::BALL_HEIGHT);
			this->stacks[col][i + 1] = std::shared_ptr<Ball>(nullptr);
		}
	}
}

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

std::shared_ptr<Ball> Scale::getAndRemoveBallFromTop(int col) {
	int i;
	for (i = STACK_HEIGHT - 1; i >= 0; i--) {
		if (this->stacks[col][i]) {
			std::shared_ptr<Ball> retBall = stacks[col][i];
			stacks[col][i] = std::shared_ptr<Ball>(nullptr);
			return retBall;
		}
	}
	return std::shared_ptr<Ball>(nullptr);
}

void Scale::setBallToPos(int row, int col) {
	// TODO: 180 is the "Y-Offset" + 20 for visual reasons and 70 for a ball that can fly there
	this->stacks[row][col]->setPos(this->leftOffset + col * Ball::BALL_WIDTH, this->topOffset + 250 + (STACK_HEIGHT - row) * Ball::BALL_HEIGHT);
}

#include "field.h"
#include <sstream>

Field::Field(SDL_Renderer *gRenderer, TTF_Font *gFont) {
	this->renderer = gRenderer;
	this->font = gFont;
	level = 4;
	ballsPlaced = 0;
	points = 0;

	this->loader = new Loader(gRenderer);

	//newBalls = malloc(size_of(Ball) * NEW_BALLS_ROWS * ROW_LENGTH);
	//stackedBalls = malloc(size_of(Ball) * STACK_HEIGHT * ROW_LENGTH);
	int i, j;
	// init text textures for weights and weights
	for (i = 0; i < COL_COUNT; i++) {
		weights[i] = 0;
		weightTextures[i] = new LTexture(gRenderer);
		weightTextures[i]->setFont(gFont);
		this->setTextFromIntForTexture(weightTextures[i], weights[i]);
	}

	SDL_Color textColor = {0, 0, 0};

	ballsPlacedTextTexture = new LTexture(gRenderer);
	ballsPlacedTextTexture->setFont(gFont);
	if (!ballsPlacedTextTexture->loadFromRenderedText("balls:", textColor)) {
		printf("failed to load from rendered text\n");
	}
	pointsTextTexture = new LTexture(gRenderer);
	pointsTextTexture->setFont(gFont);
	if (!pointsTextTexture->loadFromRenderedText("points:", textColor)) {
		printf("failed to load from rendered text\n");
	}

	ballsPlacedTexture = new LTexture(gRenderer);
	ballsPlacedTexture->setFont(gFont);
	this->setTextFromIntForTexture(ballsPlacedTexture, ballsPlaced);
	pointsTexture = new LTexture(gRenderer);
	pointsTexture->setFont(gFont);
	this->setTextFromIntForTexture(pointsTexture, points);


	// init top ball row)
	for (i = 0; i < NEW_BALLS_ROWS; i++) {
		for (j = 0; j < ROW_LENGTH; j++) {
			newBalls[i][j] = makeNewBall();
			newBalls[i][j]->setPos(LEFT_OFFSET + j * Ball::BALL_WIDTH, TOP_HEIGHT + i * Ball::BALL_HEIGHT);
		}
	}
	// init ball stacks
	for (i = 0; i < STACK_HEIGHT; i++) {
		for (j = 0; j < ROW_LENGTH; j++) {
			stackedBalls[i][j] = NULL;
		}
	}

}

Field::~Field() {
	delete this->loader;
	int i, j;
	// init top ball row
	for (i = 0; i < NEW_BALLS_ROWS; i++) {
		for (j = 0; j < ROW_LENGTH; j++) {
			if (newBalls[i][j] != NULL) {
				delete newBalls[i][j];
			}
		}
	}
	// render ball stacks
	for (i = 0; i < STACK_HEIGHT; i++) {
		for (j = 0; j < ROW_LENGTH; j++) {
			if (stackedBalls[i][j] == NULL) {
				delete stackedBalls[i][j];
			}
		}
	}
}

void Field::render() {
	this->loader->render();
	int i, j;
	// render new balls
	for (i = 0; i < NEW_BALLS_ROWS; i++) {
		for (j = 0; j < ROW_LENGTH; j++) {
			if (newBalls[i][j] != NULL) {
				newBalls[i][j]->render();
			}
		}
	}
	// render ball stacks
	for (i = 0; i < STACK_HEIGHT; i++) {
		for (j = 0; j < ROW_LENGTH; j++) {
			if (stackedBalls[i][j] != NULL) {
				stackedBalls[i][j]->render();
			}
		}
	}
	// render ball weight
	for (i = 0; i < COL_COUNT; i++) {
		// TODO: 180 is the "Y-Offset" + 20 for visual reasons and 70 for a ball that can fly there + 20 for scale + 40 for mid
		weightTextures[i]->render(LEFT_OFFSET + 15 + i * Ball::BALL_WIDTH, TOP_HEIGHT + 300 + STACK_HEIGHT * Ball::BALL_HEIGHT);
	}

	ballsPlacedTextTexture->render(BALLS_PLACED_TEXT_TEXTURE_X_POS, BALLS_PLACED_TEXT_TEXTURE_Y_POS);
	pointsTextTexture->render(POINTS_TEXT_TEXTURE_X_POS, POINTS_TEXT_TEXTURE_Y_POS);
	ballsPlacedTexture->render(BALLS_PLACED_TEXTURE_X_POS, BALLS_PLACED_TEXTURE_Y_POS);
	pointsTexture->render(POINTS_TEXTURE_X_POS, POINTS_TEXTURE_Y_POS);
}

Ball* Field::makeNewBall() {
	return new Ball(this->renderer, this->font, level);
}

Ball* Field::getBallFromNew(int col) {
	Ball* retBall = newBalls[1][col];
	newBalls[1][col] = newBalls[0][col];
	newBalls[1][col]->setYPos(newBalls[1][col]->getYPos() + Ball::BALL_HEIGHT);
	newBalls[0][col] = makeNewBall();
	newBalls[0][col]->setPos(LEFT_OFFSET + col * Ball::BALL_WIDTH, TOP_HEIGHT);
	return retBall;
}

int Field::handleKeyEvents(SDL_Event &e) {
	int retVal = 0;
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		//Adjust the velocity
		switch (e.key.keysym.sym) {
			case SDLK_LEFT:
				loader->move(0);
				break;
			case SDLK_RIGHT:
				loader->move(1);
				break;
			case SDLK_DOWN:
				int currentCol = this->loader->getCurrentCol();
				// drop ball currently in loader
				Ball* ball = this->loader->dropBall();
				if (ball != NULL) {
					int dropRow = this->dropBall(ball, currentCol);
					// everything's fine
					if (dropRow < STACK_HEIGHT) {
						if(++this->ballsPlaced % 50 == 0) {
							// drop a star
							level++;
						};
						this->setTextFromIntForTexture(ballsPlacedTexture, ballsPlaced);
						checkField(dropRow, currentCol);
					} else {
						retVal = 1;
					}
				}
				//get loader a new ball
				this->loader->giveBall(getBallFromNew(currentCol));
		}
	}
	return retVal;
}

// returns the row
int Field::dropBall(Ball* ball, int col) {
	int i, j;
	for (i = 0; i < STACK_HEIGHT; i++) {
		if (stackedBalls[i][col] == NULL) {
			stackedBalls[i][col] = ball;
			this->setBallToPos(i, col);
			this->weights[col] += ball->getWeight();
			this->setTextFromIntForTexture(weightTextures[col], weights[col]);
			return i;
		}
	}
	return STACK_HEIGHT;
}

void Field::checkField(int row, int col) {
	//printf("check for new ball at row: %d, and col: %d\n", row, col);
	if (this->checkForStacking(row, col)) {
		this->stackBalls(row, col);
		// current ball of interest is stacked, so 4 places down
		row -= 4;
	}

	if (this->checkPosForDestroying(row, col)) {
		int count = 0;
		int sumWeight = 0;
		this->destroyBalls(row, col, stackedBalls[row][col]->getColor(), &count, &sumWeight);
		this->calculateAndAddPoints(count, sumWeight);
		this->collapseField();
	}
	while (this->checkAndDestroy()) {
		this->collapseField();
	}
}

bool Field::checkForStacking(int row, int col) {
	int color = stackedBalls[row][col]->getColor();
	int i;
	int sameColor = 1;
	// check for collapse (5 balls with same color)
	// only check if stack has at least 5 balls
	if (row >= 4) {
		for (i = (row - 1); i >= (row - 4); i--) {
			if (stackedBalls[i][col]->getColor() == color) {
				sameColor++;
			} else {
				break;
			}
		}
	}
	return (sameColor == 5);
}

void Field::stackBalls(int row, int col) {
	int sumWeight = 0;
	int i;
	for (i = row; i > (row - 4); i--) {
		sumWeight += stackedBalls[i][col]->getWeight();
		stackedBalls[i][col]->collapse();
		delete stackedBalls[i][col];
		stackedBalls[i][col] = NULL;
	}
	stackedBalls[row - 4][col]->addWeight(sumWeight);
}

bool Field::checkPosForDestroying(int row, int col) {
	// check for threeway (höhö)
	bool isDestroyable = false;
	int color = stackedBalls[row][col]->getColor();

	// xxo
	if (col > 1
		&& stackedBalls[row][col - 1] != NULL && stackedBalls[row][col - 1]->getColor() == color
		&& stackedBalls[row][col - 2] != NULL && stackedBalls[row][col - 2]->getColor() == color)
		isDestroyable = true;
	// xox
	if (col > 0 && col < (COL_COUNT - 1)
		&& stackedBalls[row][col - 1] != NULL && stackedBalls[row][col - 1]->getColor() == color
		&& stackedBalls[row][col + 1] != NULL && stackedBalls[row][col + 1]->getColor() == color)
		isDestroyable = true;
	// oxx
	if (col < (COL_COUNT - 2)
		&& stackedBalls[row][col + 1] != NULL && stackedBalls[row][col + 1]->getColor() == color
		&& stackedBalls[row][col + 2] != NULL && stackedBalls[row][col + 2]->getColor() == color)
		isDestroyable = true;

	return isDestroyable;
}

bool Field::checkAndDestroy() {
	bool destroyed = false;
	int i, j, color, sameColorCount;

	for (i = 0; i < STACK_HEIGHT; i++) {
		sameColorCount = 0;
		color = -1;
		for (j = 0; j < COL_COUNT; j++) {
			if (stackedBalls[i][j] == NULL) {
				color = -1;
				sameColorCount = 0;
				continue;
			}
			if (color < 0) {
				color = stackedBalls[i][j]->getColor();
			}
			if (stackedBalls[i][j]->getColor() == color) {
				sameColorCount++;
			} else {
				if (sameColorCount > 2) {
					int count = 0;
					int weight = 0;
					this->destroyBalls(i, j - 1, color, &count, &weight);
					this->calculateAndAddPoints(count, weight);
					destroyed = true;
				}
				// reset count, reset color for new init
				sameColorCount = 0;
				if (j < (COL_COUNT - 1)) {
					// only reset color when not last row
					color = -1;
				}
			}
		}
		if (sameColorCount > 2) {
			// most right column
			int count = 0;
			int weight = 0;
			this->destroyBalls(i, COL_COUNT - 1, color, &count, &weight);
			this->calculateAndAddPoints(count, weight);
			destroyed = true;
		}
	}

	return destroyed;
}

void Field::destroyBalls(int row, int col, int color, int *count, int *sumWeight) {
	//printf("color: %d\n", color);
	int i;
	if (stackedBalls[row][col] != NULL) {
		// remove weight from stack
		this->weights[col] -= stackedBalls[row][col]->getWeight();
		//this->setWeightForWeightTexture(col);
		this->setTextFromIntForTexture(weightTextures[col], weights[col]);

		// add weight and increase count
		(*count)++;
		*sumWeight += stackedBalls[row][col]->getWeight();

		stackedBalls[row][col]->destroy();
		delete stackedBalls[row][col];
		stackedBalls[row][col] = NULL;

		if ((row + 1) < STACK_HEIGHT) {
			if (stackedBalls[row + 1][col] != NULL && stackedBalls[row + 1][col]->getColor() == color) {
				this->destroyBalls(row + 1, col, color, count, sumWeight);
			}
		}
		if ((col + 1) < COL_COUNT) {
			if (stackedBalls[row][col + 1] != NULL && stackedBalls[row][col + 1]->getColor() == color) {
				this->destroyBalls(row, col + 1, color, count, sumWeight);
			}
		}
		if ((row - 1) >= 0) {
			if (stackedBalls[row - 1][col] != NULL && stackedBalls[row - 1][col]->getColor() == color) {
				this->destroyBalls(row - 1, col, color, count, sumWeight);
			}
		}
		if ((col - 1) >= 0) {
			if (stackedBalls[row][col - 1] != NULL && stackedBalls[row][col - 1]->getColor() == color) {
				this->destroyBalls(row, col - 1, color, count, sumWeight);
			}
		}
	}
}

void Field::collapseField() {
	int i;
	for (i = 0; i < COL_COUNT; i++) {
		this->collapseStack(0, i);
	}
}

void Field::collapseStack(int row, int col) {
	if ((row + 1) < STACK_HEIGHT) {
		if (stackedBalls[row][col] == NULL) {
			int i;
			int grabRow = -1;
			for (i = row + 1; i < STACK_HEIGHT; i++) {
				if (stackedBalls[i][col] != NULL) {
					grabRow = i;
					break;
				}
			}
			if (grabRow > 0) {
				stackedBalls[row][col] = stackedBalls[grabRow][col];
				this->setBallToPos(row, col);
				stackedBalls[grabRow][col]->collapse();
				stackedBalls[grabRow][col] = NULL;
			}
		}
		this->collapseStack(row + 1, col);
	}
}

void Field::calculateAndAddPoints(int count, int weight) {
	points += count * weight;
	this->setTextFromIntForTexture(pointsTexture, points);
}

void Field::setBallToPos(int row, int col) {
	// TODO: 180 is the "Y-Offset" + 20 for visual reasons and 70 for a ball that can fly there
	stackedBalls[row][col]->setPos(LEFT_OFFSET + col * Ball::BALL_WIDTH, TOP_HEIGHT + 250 + (STACK_HEIGHT - row) * Ball::BALL_HEIGHT);
}

void Field::setTextFromIntForTexture(LTexture *texture, int val) {
	SDL_Color textColor = {0, 0, 0};
	std::stringstream ss1;
	ss1 << val;
	const char *chWeight = ss1.str().c_str();
	if (!texture->loadFromRenderedText(chWeight, textColor)) {
		printf("failed to load from rendered text\n");
	}
}

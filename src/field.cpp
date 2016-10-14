#include "field.h"

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
	// init top ball row)
	for (i = 0; i < NEW_BALLS_ROWS; i++) {
		for (j = 0; j < ROW_LENGTH; j++) {
			newBalls[i][j] = makeNewBall();
			newBalls[i][j]->setPos(LEFT_OFFSET + j * Ball::BALL_WIDTH, i * Ball::BALL_HEIGHT);
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
}

Ball* Field::makeNewBall() {
	return new Ball(this->renderer, this->font, level);
}

Ball* Field::getBallFromNew(int col) {
	Ball* retBall = newBalls[1][col];
	newBalls[1][col] = newBalls[0][col];
	newBalls[1][col]->setYPos(newBalls[1][col]->getYPos() + Ball::BALL_HEIGHT);
	newBalls[0][col] = makeNewBall();
	newBalls[0][col]->setPos(LEFT_OFFSET + col * Ball::BALL_WIDTH, 0);
	return retBall;
}

int Field::handleKeyEvents(SDL_Event &e) {
	int retVal = 0;
	if (e.type == SDL_KEYUP && e.key.repeat == 0) {
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
						this->ballsPlaced++;
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
			//stackedBalls[i][col]->setPos(LEFT_OFFSET + col * Ball::BALL_WIDTH, 250 + (STACK_HEIGHT - i) * Ball::BALL_HEIGHT);
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

	// need to rewrite, checkForDestroying needs to get through the whole stackedBalls for chain destroying
	if (this->checkPosForDestroying(row, col)) {
		this->destroyBalls(row, col, stackedBalls[row][col]->getColor());
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

void Field::destroyBalls(int row, int col, int color) {
	int i;
	if (stackedBalls[row][col] != NULL) {
		stackedBalls[row][col]->destroy();
		delete stackedBalls[row][col];
		stackedBalls[row][col] = NULL;

		if ((row + 1) < STACK_HEIGHT) {
			if (stackedBalls[row + 1][col] != NULL && stackedBalls[row + 1][col]->getColor() == color) {
				this->destroyBalls(row + 1, col, color);
			}
		}
		if ((col + 1) < COL_COUNT) {
			if (stackedBalls[row][col + 1] != NULL && stackedBalls[row][col + 1]->getColor() == color) {
				this->destroyBalls(row, col + 1, color);
			}
		}
		if ((row - 1) >= 0) {
			if (stackedBalls[row - 1][col] != NULL && stackedBalls[row - 1][col]->getColor() == color) {
				this->destroyBalls(row - 1, col, color);
			}
		}
		if ((col - 1) >= 0) {
			if (stackedBalls[row][col - 1] != NULL && stackedBalls[row][col - 1]->getColor() == color) {
				this->destroyBalls(row, col - 1, color);
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

void Field::setBallToPos(int row, int col) {
	// TODO: 180 is the "Y-Offset" + 20 for visual reasons and 70 for a ball that can fly there
	stackedBalls[row][col]->setPos(LEFT_OFFSET + col * Ball::BALL_WIDTH, 250 + (STACK_HEIGHT - row) * Ball::BALL_HEIGHT);
}

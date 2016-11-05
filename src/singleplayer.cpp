#include "singleplayer.h"
#include <iostream>

Singleplayer::Singleplayer(SDL_Renderer *gRenderer, TTF_Font *gFont) : renderer(gRenderer), font(gFont) {
	this->level = 4;
	this->ballsPlaced = 0;
	this->points = 0;


	//this->font = std::make_shared<SDL_Font>(TTF_OpenFont("resources/fonts/Roboto-Bold.ttf", 28));
	/*if (!gFont) {
		std::cout << "Failed to load font, error: " << TTF_GetError() << std::endl;
		exit(13);
	}*/

	this->loader = std::make_unique<Loader>(gRenderer);
	int wat = TOP_HEIGHT;
	int wat2 = TOP_LEFT_OFFSET;
	this->ballSource = std::make_unique<BallSource>(gRenderer, this->font, wat2, wat);
	wat = FIELD_TOP_OFFSET;
	wat2 = FIELD_LEFT_OFFSET;
	this->field = std::make_unique<Field>(gRenderer, this->font, wat2, wat);

	SDL_Color textColor = {0, 0, 0};

	ballsPlacedTextTexture = std::make_unique<LTexture>(gRenderer);
	ballsPlacedTextTexture->setFont(gFont);
	if (!ballsPlacedTextTexture->loadFromRenderedText("balls:", textColor)) {
		std::cout << "failed to load from rendered text" << std::endl;
	}
	pointsTextTexture = std::make_unique<LTexture>(gRenderer);
	pointsTextTexture->setFont(gFont);
	if (!pointsTextTexture->loadFromRenderedText("points:", textColor)) {
		std::cout << "failed to load from rendered text" << std::endl;
	}

	ballsPlacedTexture = std::make_unique<LTexture>(gRenderer);;
	ballsPlacedTexture->setFont(gFont);
	this->ballsPlacedTexture->setTextFromInt(ballsPlaced);
	pointsTexture = std::make_unique<LTexture>(gRenderer);;
	pointsTexture->setFont(gFont);
	this->pointsTexture->setTextFromInt(points);
}

Singleplayer::~Singleplayer() {
	fontColor.reset();
	ballsPlacedTexture.reset();
	ballsPlacedTextTexture.reset();
	pointsTexture.reset();
	pointsTextTexture.reset();
}

int Singleplayer::handleKeyEvents(SDL_Event &e) {
	int retVal = 0;
	if (e.type == SDL_KEYDOWN) {
		//Adjust the velocity
		switch (e.key.keysym.sym) {
			case SDLK_LEFT:
				this->loader->move(0);
				break;
			case SDLK_RIGHT:
				this->loader->move(1);
				break;
			case SDLK_DOWN:
				int currentCol = this->loader->getCurrentCol();
				// drop ball currently in loader
				std::shared_ptr<Ball> ball = this->loader->dropBall();
				if (ball) {
					// everything's fine
					if (this->field->dropBallAt(ball, currentCol)) {
						if(++this->ballsPlaced % 50 == 0) {
							// drop a star
							this->level++;
						};
						this->ballsPlacedTexture->setTextFromInt(ballsPlaced);
						this->field->check();
					} else {
						retVal = 1;
					}
				}
				//get loader a new ball
				this->loader->giveBall(this->ballSource->getBallAt(currentCol, this->level));
		}
	}
	return retVal;
}

void Singleplayer::render() {
	this->loader->render();
	this->ballSource->render();
	this->field->render();

	this->ballsPlacedTextTexture->render(BALLS_PLACED_TEXT_TEXTURE_X_POS, BALLS_PLACED_TEXT_TEXTURE_Y_POS);
	this->pointsTextTexture->render(POINTS_TEXT_TEXTURE_X_POS, POINTS_TEXT_TEXTURE_Y_POS);
	this->ballsPlacedTexture->render(BALLS_PLACED_TEXTURE_X_POS, BALLS_PLACED_TEXTURE_Y_POS);
	this->pointsTexture->render(POINTS_TEXTURE_X_POS, POINTS_TEXTURE_Y_POS);
}

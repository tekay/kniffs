#include "singleplayer.h"
#include <iostream>
#include "logger.h"

Singleplayer::Singleplayer(SDL_Renderer *gRenderer, TTF_Font *gFont) : renderer(gRenderer), font(gFont) {
	this->level = 4;
	this->ballsPlaced = std::make_shared<unsigned int>(0);
	this->points = std::make_shared<unsigned int>(0);

	this->loader = std::make_unique<Loader>(gRenderer);
	int wat = TOP_HEIGHT;
	int wat2 = TOP_LEFT_OFFSET;
	this->ballSource = std::make_unique<BallSource>(gRenderer, this->font, this->ballsPlaced, wat2, wat);
	wat = FIELD_TOP_OFFSET;
	wat2 = FIELD_LEFT_OFFSET;
	this->field = std::make_unique<Field>(gRenderer, this->font, this->points, wat2, wat);

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
	this->ballsPlacedTexture->setTextFromInt(*this->ballsPlaced);
	pointsTexture = std::make_unique<LTexture>(gRenderer);;
	pointsTexture->setFont(gFont);
	this->pointsTexture->setTextFromInt(*this->points);
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
						if((++*this->ballsPlaced) % 50 == 0) {
							// drop a star
							Logger::info("level up! level is now: " + std::to_string(this->level));
							this->level++;
						};
						this->ballsPlacedTexture->setTextFromInt(*this->ballsPlaced);
						if (!this->field->check()) {
							retVal = 1;
						}
					} else {
						retVal = 1;
					}
				}
				this->pointsTexture->setTextFromInt(*this->points);
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

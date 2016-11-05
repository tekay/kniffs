#include <iostream>

#include "event.h"

Event::Event() {
	this->type = NOTHING;
}

Event::Event(Type type) : type(type) {}

Event::~Event() {

}

int Event::getType() {
	return this->type;
}

void Event::setType(Type type) {
	this->type = type;
}

std::shared_ptr<Ball> Event::getBall() {
	if (this->type == THROW_BALL) {
		return this->ball;
	} else {
		std::cout << "No Ball in Event!" << std::endl;
		return std::shared_ptr<Ball>();
	}
}

void Event::setBall(std::shared_ptr<Ball> ball) {
	if (this->type == THROW_BALL) {
		this->ball = ball;
	}
}

int Event::getDistance() {
	if (this->type == THROW_BALL) {
		return this->distance;
	}
}

void Event::setDistance(int distance) {
	if (this->type == THROW_BALL) {
		this->distance = distance;
	}
}

int Event::getDirection() {
	if (this->type == THROW_BALL) {
		return this->direction;
	}
}

void Event::setDirection(int direction) {
	if (this->type == THROW_BALL) {
		this->direction = direction;
	}
}

#ifndef EVENT_H__
#define EVENT_H__

#include <memory>
#include "ball.h"

class Event {
	public:
		enum Type {
			NOTHING = 0, THROW_BALL = 1, LOSS = 2
		};
		Event();
		Event(Type type);
		~Event();

		int getType();
		void setType(Type type);
		std::shared_ptr<Ball> getBall();
		void setBall(std::shared_ptr<Ball> ball);
		int getDistance();
		void setDistance(int distance);
		int getDirection();
		void setDirection(int direction);

	private:
		std::shared_ptr<Ball> ball;
		Type type;
		int distance;
		// direction: 1 = left, 2 = right
		int direction;

};

#endif

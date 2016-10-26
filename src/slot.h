#ifndef SLOT_H__
#define SLOT_H__

#define SLOT_BALL 0
#define SLOT_SCALE 1
#define SLOT_SPECIAL 2

class Slot {
	public:
		virtual int getType() = 0;

		virtual void move() = 0;
		virtual void render() = 0;
		virtual void setXPos(int x) = 0;
		virtual int getXPos() = 0;
		virtual void setYPos(int y) = 0;
		virtual int getYPos() = 0;
		virtual void setPos(int x, int y) = 0;
		virtual int getColor() = 0;
		virtual void setWeight(int w) = 0;
		virtual int getWeight() = 0;
		virtual void addWeight(int w) = 0;

		// graphical stuff
		virtual void collapse() = 0;
		virtual void destroy() = 0;
};

#endif SLOT_H__

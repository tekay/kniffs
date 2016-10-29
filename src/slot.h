#ifndef SLOT_H__
#define SLOT_H__

#define SLOT_BALL 0
#define SLOT_SCALE 1
#define SLOT_SPECIAL 2

class Slot {
	public:
		static const int BALL = 0;
		static const int SCALE = 1;
		static const int SPECIAL = 2;
		virtual int getType() = 0;
};

#endif SLOT_H__

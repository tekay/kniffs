#ifndef SCALE_H__
#define SCALE_H__

#include "slot.h"
#include "ltexture.h"

class Scale : Slot {
	public:
		Scale(SDL_Renderer *gRenderer);
		~Scale();

		int getType();

		void render();
	private:
		// status: 0 = neutral, 1 = left down, 2 = right down
		int status = 0;

		LTexture *texture;
};

#endif SCALE_H__

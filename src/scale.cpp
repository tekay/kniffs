#include "scale.h"
#include "slot.h"
#include "ltexture.h"

Scale::Scale(SDL_Renderer *gRenderer) {
	texture = new LTexture(gRenderer);
	if (!texture->loadFromFile("resources/scale_neutral.png")) {
		printf("error loading scale png\n");
		exit(13);
	}
}

Scale::~Scale() {

}

int Scale::getType() {
	return SLOT_SCALE;
}

void Scale::render() {
	
}

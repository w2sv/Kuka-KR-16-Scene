#include "textures.h"


const char* filenames[NUM_TEXTURES] = {
	"steelplate.bmp",
};


void initTextures() {
	const char* FILE_PATH = ".\\textures\\";

	for (size_t i = 0; i < NUM_TEXTURES; i++) {
		if (!textures[i].load(concatenatedCharPtr(FILE_PATH, filenames[i]), true)) {
			std::cerr << "Couldn't open texture #" << i << std::endl;
			std::exit(1);
		}

		textures[i].setMinFilter(GL_NEAREST);
		textures[i].setMagFilter(GL_LINEAR);
		textures[i].setWrapMode(GL_CLAMP);
	}
}
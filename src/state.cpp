#include "state.h"

#include <time.h>


double GlobalState::screenSize[2] = { NULL };
GLenum GlobalState::drawMode = GL_FILL;
bool GlobalState::lightMode = true;
int GlobalState::normalMode = 0;
bool GlobalState::cullMode = false;
bool GlobalState::drawCoordSystem = false;
bool GlobalState::displayHelp = true;
bool GlobalState::displayFps = true;
float GlobalState::fps = NULL;


void GlobalState::updateFps() {
	static time_t lastTime = 0;
	static int frames = 0;

	time_t now;
	time(&now);

	if (now - lastTime >= 1) {
		fps = ((float)frames) / (float)(now - lastTime);

		lastTime = now;
		frames = 0;
	}
	else
		frames++;
}
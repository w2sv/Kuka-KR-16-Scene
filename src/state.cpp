#include "state.h"

#include "utils.h"

#include <time.h>



double GlobalState::screenSize[2] = { NULL };
GLenum GlobalState::drawMode = GL_FILL;
GLenum GlobalState::shadingMode = GL_SMOOTH;
bool GlobalState::lightMode = true;
int GlobalState::normalMode = 0;
bool GlobalState::cullMode = false;
bool GlobalState::drawCoordSystem = false;
bool GlobalState::displayHelp = false;
bool GlobalState::displayFps = true;
float GlobalState::fps = NULL;
bool GlobalState::sunlight = false;


void GlobalState::updateFps() {
	static time_t lastTime = 0;
	static int frames = 0;

	time_t now = currentSecondsTimestamp();
	if (now - lastTime >= 1) {
		fps = ((float)frames) / (float)(now - lastTime);

		lastTime = now;
		frames = 0;
	}
	else
		frames++;
}
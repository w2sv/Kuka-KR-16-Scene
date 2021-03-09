#include "state.h"


double GlobalState::screenSize[2] = { 0 };
GLenum GlobalState::drawMode = GL_FILL;
bool GlobalState::lightMode = true;
int GlobalState::normalMode = 0;
bool GlobalState::cullMode = false;
bool GlobalState::drawCoordSystem = false;
bool GlobalState::drawHelp = false;
bool GlobalState::drawFps = true;
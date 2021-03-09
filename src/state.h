#pragma once

#include "../dependencies/freeglut.h"


struct GlobalState {
	static double screenSize[2];
	static GLenum drawMode;
	static bool lightMode;
	static int normalMode;
	static bool cullMode;

	static bool drawCoordSystem;
	static bool drawHelp;
	static bool drawFps;
};
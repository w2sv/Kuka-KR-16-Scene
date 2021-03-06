#pragma once

#include "../dependencies/freeglut.h"


struct GlobalState {
	static double screenSize[2];
	static GLenum drawMode;
	static GLenum shadingMode;
	static bool lightMode;
	static int normalMode;
	static bool cullMode;

	static bool sunlight;

	static bool drawCoordSystem;
	static bool displayHelp;
	static bool displayFps;
	static float fps;

	static void updateFps();
};
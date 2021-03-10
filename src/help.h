#pragma once

#ifndef HELP_H
#define HELP_H

#include "../dependencies/freeglut.h"

#include "utils.h"
#include "glutils.h"


void displayHelp();

void displayFps();


namespace Text {
	static void* DEFAULT_FONT = GLUT_BITMAP_HELVETICA_18;

	void display(const Vector2& position, const char* text, void* font = DEFAULT_FONT);
	
	void displayColored(const Vector2& position, const char* text, Color& color, void* font = DEFAULT_FONT);
	
	void displayWithShadow(const Vector2& position, const char* text, Color& color, float shadow, void* font = DEFAULT_FONT);

	struct OrthogonalProjection {
		OrthogonalProjection();

		void activate(bool alterDepthTest);
		void deactivate(bool alterDepthTest);
	private:
		GLfloat currentColor[4];
	};
}
#endif
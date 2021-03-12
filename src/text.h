#pragma once

#include "utils.h"
#include "glutils.h"

#include "../dependencies/freeglut.h"


namespace Text {
	static void* DEFAULT_FONT = GLUT_BITMAP_HELVETICA_18;

	void display(const Vector2& position, const char* text, void* font = DEFAULT_FONT);

	void displayColored(const Vector2& position, const char* text, const Color& color, void* font = DEFAULT_FONT);

	void displayWithShadow(const Vector2& position, const char* text, const Color& color, float shadow, void* font = DEFAULT_FONT);

	struct OrthogonalProjection {
		OrthogonalProjection();

		void activate(bool alterDepthTest);

		void deactivate(bool alterDepthTest);
	private:
		GLfloat currentColor[4];
	};
}
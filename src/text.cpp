#include "text.h"

#include "utils.h"



void Text::display(const Vector2& position, const char* text, void* font) {
	glRasterPos2f(position.x, position.y);
	glutBitmapString(font, reinterpret_cast<const unsigned char*>(text));
}


void Text::displayColored(const Vector2& position, const char* text, const Color& color, void* font) {
	color.render(false);
	display(position, text, font);
}


void Text::displayWithShadow(const Vector2& position, const char* text, const Color& color, float shadow, void* font) {
	displayColored(Vector2(position.x + shadow, position.y - shadow), text, Color(COLORS::BLACK), font);
	displayColored(position, text, color, font);
}


////////////////////////////////////////////////////////////
/// .OrthogonalProjection
////////////////////////////////////////////////////////////
Text::OrthogonalProjection::OrthogonalProjection() :
	currentColor{ NULL }
{}


void Text::OrthogonalProjection::activate(bool alterDepthTest) {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glGetFloatv(GL_CURRENT_COLOR, currentColor);

	//orthogonale projektion setzen
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	if (alterDepthTest)
		glDisable(GL_DEPTH_TEST);
}


void Text::OrthogonalProjection::deactivate(bool alterDepthTest) {
	if (alterDepthTest)
		glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	// reset matrices
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glColor4fv(currentColor);
	glPopAttrib();

	// reset color
	memset(currentColor, NULL, 4);
}
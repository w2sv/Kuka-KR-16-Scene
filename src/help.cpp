#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "state.h"
#include "utils.h"

#include "help.h"



////////////////////////////////////////////////////////////
/// Text
////////////////////////////////////////////////////////////
void Text::display(const Vector2& position, const char* text, void* font){
	glRasterPos2f(position.x, position.y);
	for (size_t i = 0; i < strlen(text); ++i)
		glutBitmapCharacter(font, text[i]);
}



void Text::displayColored(const Vector2& position, const char* text, Color& color, void* font){
	color.render(false);
	display(position, text, font);
}



void Text::displayWithShadow(const Vector2& position, const char* text, Color& color, float shadow, void* font){
	displayColored(Vector2(position.x + shadow, position.y - shadow), text, Color(Colors::BLACK), font);
	displayColored(position, text, color, font);
}



////////////////////////////////////////////////////////////
/// .OrthogonalProjection
////////////////////////////////////////////////////////////
Text::OrthogonalProjection::OrthogonalProjection():
	currentColor{NULL}
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



////////////////////////////////////////////////////////////
/// Display functions
////////////////////////////////////////////////////////////
void drawBackground(){
	static const float CORNER_COORD = 0.8f;

	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.3f, 0.3f, 0.3f, 0.1f);
	glBegin(GL_QUADS);
		glVertex2f(-CORNER_COORD, CORNER_COORD);
		glVertex2f(-CORNER_COORD, -CORNER_COORD);
		glVertex2f(CORNER_COORD, -CORNER_COORD);
		glVertex2f(CORNER_COORD, CORNER_COORD);
	glEnd();
	glDisable(GL_BLEND);
}



void displaySectionTitle(const Vector2& position, const char* title) {
	Text::displayWithShadow(position, title, Color(0, 1, 0), 0.003, GLUT_BITMAP_9_BY_15);
}



void displayColumn(Vector2& startPos, const char** column, float verticalSpace, const Color& color) {
	color.render(false);
	
	size_t i = 0;
	while (true) {
		if (column[i]) {
			Text::display(startPos, column[i], GLUT_BITMAP_8_BY_13);
			startPos.y += verticalSpace;

			i++;
		}
		else
			return;
	}
}



void displayHelp() {
	

	drawBackground();

	// display help window title
	Text::displayWithShadow(Vector2(-0.6f, 0.7f), "KUKA KR 16", Color(1, 1, 0), 0.003f, GLUT_BITMAP_9_BY_15);

	static const char* FIRST_COLUMN[4] = { "1. Axis", "2. Axis", "3. Axis", "4. Axis" };

	displaySectionTitle(Vector2(-0.8, 0.5), "Robot Control");
	displayColumn(Vector2(-0.7, -0.5), FIRST_COLUMN, 0.05, Colors::WHITE);


}



void displayFps(){
	static char fpstext[9];

	sprintf(fpstext, "FPS: %.0f", GlobalState::fps);
	Text::displayWithShadow(Vector2(-0.78f, -0.78f), fpstext, Color(214, 121, 15), 0.003f, GLUT_BITMAP_HELVETICA_18);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>

#include "state.h"
#include "utils.h"

#include "help.h"



////////////////////////////////////////////////////////////
/// Text
////////////////////////////////////////////////////////////
void Text::display(const Vector2& position, const char* text, void* font){
	glRasterPos2f(position.x, position.y);
	glutBitmapString(font, reinterpret_cast<const unsigned char*>(text));
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
/// Helpers
////////////////////////////////////////////////////////////
void displaySectionTitle(const Vector2& position, const char* title) {
	Text::displayWithShadow(position, title, Color(186, 34, 64), 0.003, GLUT_BITMAP_HELVETICA_18);
}



void displayColumn(Vector2& startPos, const std::vector<char*>&& column, float verticalSpace, const Color& color) {
	color.render(false);

	for (char* entry : column) {
		Text::display(startPos, entry);
		startPos.y -= verticalSpace;
	}
}



void displayRow(Vector2& startPos, const std::vector<char*>&& row, float horizontalSpace, const Color& color) {
	color.render(false);

	for (char* entry : row) {
		Text::display(startPos, entry);
		startPos.x += horizontalSpace;
	}
}



////////////////////////////////////////////////////////////
/// Display Functions
////////////////////////////////////////////////////////////

namespace Section {
	const Color KEY_DESCRIPTION_COLOR(Colors::WHITE);
	const Color KEY_COLOR(22, 201, 133);
	
	const float VERTICAL_COL_SPACE = 0.06;

	const float FIRST_ENTRY_ROW_Y = 0.41;
	const float HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE = 0.485;

	namespace LeftHalf {
		const float KEY_DESCRIPTION_COL_X = -0.8;

		namespace Robot {
			void control() {
				displaySectionTitle(Vector2(-0.6, 0.65), "Robot Control");

				// parameter subheader row, axes designations column
				displayRow(Vector2(-0.5, 0.55), { "Angle", "Velocity" }, 0.15, KEY_DESCRIPTION_COLOR);
				displayColumn(Vector2(KEY_DESCRIPTION_COL_X, FIRST_ENTRY_ROW_Y), { "1. Axis", "2. Axis", "3. Axis", "4. Axis" }, VERTICAL_COL_SPACE, KEY_DESCRIPTION_COLOR);

				// ---------parameter subsections-----------

				const float PLUS_MIN_COL_SPACE = 0.075;
				const float PLUS_MIN_ROW_Y = 0.485;

				// angle subsection
				const float ANGLE_PLUS_COL_X = -0.515;
				const float VECLOCITY_PLUS_COL_X = -0.35;

				displayRow(Vector2(ANGLE_PLUS_COL_X, PLUS_MIN_ROW_Y), { "+", "-" }, PLUS_MIN_COL_SPACE, Colors::BLACK);

				displayColumn(Vector2(ANGLE_PLUS_COL_X, FIRST_ENTRY_ROW_Y), { "d", "w", "t", "h" }, VERTICAL_COL_SPACE, KEY_COLOR);
				displayColumn(Vector2(ANGLE_PLUS_COL_X + PLUS_MIN_COL_SPACE, FIRST_ENTRY_ROW_Y), { "a", "s", "g", "f" }, VERTICAL_COL_SPACE, KEY_COLOR);

				// velocity subsection
				displayRow(Vector2(VECLOCITY_PLUS_COL_X, PLUS_MIN_ROW_Y), { "+", "-" }, PLUS_MIN_COL_SPACE, Colors::BLACK);

				displayColumn(Vector2(VECLOCITY_PLUS_COL_X, FIRST_ENTRY_ROW_Y), { "1+", "2+", "3+", "4+" }, VERTICAL_COL_SPACE, KEY_COLOR);
				displayColumn(Vector2(VECLOCITY_PLUS_COL_X + PLUS_MIN_COL_SPACE, FIRST_ENTRY_ROW_Y), { "1-", "2-", "3-", "4-" }, VERTICAL_COL_SPACE, KEY_COLOR);
			}

			void functionality() {
				static const float START_Y = 0.1;

				displayColumn(Vector2(KEY_DESCRIPTION_COL_X, START_Y), {
					"»Reset",
					"»Snap to random configuration",
					"»Toggle tcp coord system",
					"»Approach random configuration",
					"»Toggle infinite random config-"
					}, VERTICAL_COL_SPACE, KEY_DESCRIPTION_COLOR);
				Text::display(Vector2(-0.785, -0.2), "uration approach mode");

				displayColumn(Vector2(KEY_DESCRIPTION_COL_X + HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE, START_Y + 0.005), { "F1", "F2", "F3", "F4", "F5" }, VERTICAL_COL_SPACE, KEY_COLOR);
			}
		}

		void camera() {
			displaySectionTitle(Vector2(-0.53, -0.3), "Camera");

			displayColumn(Vector2(KEY_DESCRIPTION_COL_X, -0.4), { "»Reset", "»Toggle orbit mode", "»Toggle tcp mode", "»Toggle reverse tcp mode" }, 0.06, KEY_DESCRIPTION_COLOR);
			displayColumn(Vector2(KEY_DESCRIPTION_COL_X + HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE, -0.4), { "Left", "Right", "Up", "Down" }, 0.06, KEY_COLOR);
		}
	}

	namespace RightHalf {
		const float DESCRIPTION_COL_X = 0.25;

		const float HEADER_X = 0.4;
		const float HEADER_TO_FIRST_ROW_VERTICAL_SPACE = 0.15;

		void generic() {
			displaySectionTitle(Vector2(HEADER_X, 0.65), "Generic");

			displayColumn(Vector2(DESCRIPTION_COL_X, FIRST_ENTRY_ROW_Y), { "»Quit program", "»Toggle full screen" }, VERTICAL_COL_SPACE, KEY_DESCRIPTION_COLOR);
			displayColumn(Vector2(DESCRIPTION_COL_X + HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE, FIRST_ENTRY_ROW_Y), { "q", "p" }, VERTICAL_COL_SPACE, KEY_COLOR);
		}

		void display() {
			const float HEADER_Y = 0.2;
			displaySectionTitle(Vector2(HEADER_X, HEADER_Y), "Display");

			displayColumn(Vector2(DESCRIPTION_COL_X, HEADER_Y - HEADER_TO_FIRST_ROW_VERTICAL_SPACE), { "»Toggle fps", "»Toggle global coordinate system", "»Toggle axis states" }, VERTICAL_COL_SPACE, KEY_DESCRIPTION_COLOR);
			displayColumn(Vector2(DESCRIPTION_COL_X + HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE, HEADER_Y - HEADER_TO_FIRST_ROW_VERTICAL_SPACE), { "b", "n", "m" }, VERTICAL_COL_SPACE, KEY_COLOR);
		}

		void graphics() {
			const float HEADER_Y = -0.2;
			displaySectionTitle(Vector2(HEADER_X, HEADER_Y), "Graphics");

			displayColumn(Vector2(DESCRIPTION_COL_X, HEADER_Y - HEADER_TO_FIRST_ROW_VERTICAL_SPACE), { "»Toggle wireframe mode", "»Toggle light mode", "»Toggle backfaceculling" }, VERTICAL_COL_SPACE, KEY_DESCRIPTION_COLOR);
			displayColumn(Vector2(DESCRIPTION_COL_X + HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE, HEADER_Y - HEADER_TO_FIRST_ROW_VERTICAL_SPACE), { "y", "x", "c" }, VERTICAL_COL_SPACE, KEY_COLOR);
		}
	}
}



void drawBackground() {
	static const float CORNER_COORD = 0.9f;

	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
	glBegin(GL_QUADS);
		glVertex2f(-CORNER_COORD, CORNER_COORD);
		glVertex2f(-CORNER_COORD, -CORNER_COORD);
		glVertex2f(CORNER_COORD, -CORNER_COORD);
		glVertex2f(CORNER_COORD, CORNER_COORD);
	glEnd();
	glDisable(GL_BLEND);
}



void displayHelp() {
	drawBackground();

	Text::displayWithShadow(Vector2(-0.05, 0.8f), "KUKA KR 16", Color(230, 80, 21), 0.003f, GLUT_BITMAP_HELVETICA_18);

	Section::LeftHalf::Robot::control();
	Section::LeftHalf::Robot::functionality();
	Section::LeftHalf::camera();

	Section::RightHalf::generic();
	Section::RightHalf::display();
	Section::RightHalf::graphics();
}



void displayFps(){
	static char fpstext[9];

	sprintf(fpstext, "FPS: %.0f", GlobalState::fps);
	Text::displayWithShadow(Vector2(-0.78f, -0.78f), fpstext, Color(214, 121, 15), 0.003f, GLUT_BITMAP_HELVETICA_18);
}
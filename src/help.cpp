#include "help.h"

#include "state.h"
#include "utils.h"
#include "text.h"
#include "glutils.h"

#include <vector>


void displayFps() {
	static char fpstext[9];

	sprintf(fpstext, "FPS: %.0f", GlobalState::fps);
	Text::displayWithShadow(Vector2(-0.78f, -0.78f), fpstext, Color::fromUnnormalizedValues(214, 121, 15), 0.003f, GLUT_BITMAP_HELVETICA_18);
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


namespace Section {
	const Color KEY_DESCRIPTION_COLOR(COLORS::WHITE);
	const Color KEY_COLOR = Color::fromUnnormalizedValues(22, 201, 133);
	
	const float VERTICAL_COL_SPACE = 0.06;

	const float FIRST_ENTRY_ROW_Y = 0.41;
	const float HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE = 0.485;

	////////////////////////////////////////////////////////////
	/// Helpers
	////////////////////////////////////////////////////////////

	void displaySectionTitle(const Vector2& position, const char* title) {
		Text::displayWithShadow(position, title, Color::fromUnnormalizedValues(186, 34, 64), 0.003, GLUT_BITMAP_HELVETICA_18);
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
	/// Display
	////////////////////////////////////////////////////////////

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

				displayRow(Vector2(ANGLE_PLUS_COL_X, PLUS_MIN_ROW_Y), { "+", "-" }, PLUS_MIN_COL_SPACE, COLORS::BLACK);

				displayColumn(Vector2(ANGLE_PLUS_COL_X, FIRST_ENTRY_ROW_Y), { "d", "w", "t", "h" }, VERTICAL_COL_SPACE, KEY_COLOR);
				displayColumn(Vector2(ANGLE_PLUS_COL_X + PLUS_MIN_COL_SPACE, FIRST_ENTRY_ROW_Y), { "a", "s", "g", "f" }, VERTICAL_COL_SPACE, KEY_COLOR);

				// velocity subsection
				displayRow(Vector2(VECLOCITY_PLUS_COL_X, PLUS_MIN_ROW_Y), { "+", "-" }, PLUS_MIN_COL_SPACE, COLORS::BLACK);

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


void displayHelp() {
	drawBackground();

	Text::displayWithShadow(Vector2(-0.05, 0.8f), "KUKA KR 16", Color::fromUnnormalizedValues(230, 80, 21), 0.003f, GLUT_BITMAP_HELVETICA_18);

	Section::LeftHalf::Robot::control();
	Section::LeftHalf::Robot::functionality();
	Section::LeftHalf::camera();

	Section::RightHalf::generic();
	Section::RightHalf::display();
	Section::RightHalf::graphics();
}
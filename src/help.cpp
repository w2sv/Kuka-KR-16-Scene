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
	typedef const std::vector<char*>&& CharPtrs;

	////////////////////////////////////////////////////////////
	/// Helpers
	////////////////////////////////////////////////////////////

	void displaySectionTitle(const Vector2& position, const char* title) {
		Text::displayWithShadow(position, title, Color::fromUnnormalizedValues(186, 34, 64), 0.003, GLUT_BITMAP_HELVETICA_18);
	}


	void displayColumn(Vector2& startPos, CharPtrs column, float verticalSpace, const Color& color) {
		color.render(false);

		for (char* entry : column) {
			Text::display(startPos, entry);
			startPos.y -= verticalSpace;
		}
	}


	void displayRow(Vector2& startPos, CharPtrs row, float horizontalSpace, const Color& color) {
		color.render(false);

		for (char* entry : row) {
			Text::display(startPos, entry);
			startPos.x += horizontalSpace;
		}
	}

	////////////////////////////////////////////////////////////
	/// Display
	////////////////////////////////////////////////////////////

	static const Color KEY_DESCRIPTION_COLOR(COLORS::WHITE);
	static const Color KEY_COLOR = Color::fromUnnormalizedValues(22, 201, 133);

	static const float VERTICAL_COL_SPACE = 0.06;

	static const float FIRST_ENTRY_ROW_Y = 0.41;
	static const float HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE = 0.485;

	static const float HEADER_TO_FIRST_ROW_VERTICAL_SPACE = 0.1;

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
					"»Toggle tcp coord system",
					"»Approach home position",
					"»Snap to home position",
					"»Approach random configuration",
					"»Snap to random configuration",
					"»Toggle endless random config-"
					}, VERTICAL_COL_SPACE, KEY_DESCRIPTION_COLOR);
				Text::display(Vector2(KEY_DESCRIPTION_COL_X + 0.018, -0.26), "uration approach mode");

				displayColumn(Vector2(KEY_DESCRIPTION_COL_X + HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE, START_Y + 0.005), { "F1", "F2", "F2 Shift", "F3", "F3 Shift", "F4"}, VERTICAL_COL_SPACE, KEY_COLOR);

				// display surplus velocity resetting key description
				Text::displayColored(Vector2(KEY_DESCRIPTION_COL_X, -0.32), "»Reset velocities", KEY_DESCRIPTION_COLOR);
				Text::displayColored(Vector2(KEY_DESCRIPTION_COL_X + HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE, -0.32), "F5", KEY_COLOR);
			}
		}

		void camera() {
			static const float HEADER_Y = -0.4;

			displaySectionTitle(Vector2(-0.53, HEADER_Y), "Camera");

			displayColumn(Vector2(KEY_DESCRIPTION_COL_X, HEADER_Y - HEADER_TO_FIRST_ROW_VERTICAL_SPACE), { "»Reset", "»Toggle orbit mode", "»Toggle tcp mode", "»Toggle reverse tcp mode" }, VERTICAL_COL_SPACE, KEY_DESCRIPTION_COLOR);
			displayColumn(Vector2(KEY_DESCRIPTION_COL_X + HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE, HEADER_Y - HEADER_TO_FIRST_ROW_VERTICAL_SPACE), { "Left", "Right", "Up", "Down" }, VERTICAL_COL_SPACE, KEY_COLOR);
		}
	}


	namespace RightHalf {
		void displaySection(float headerY, const char* title, CharPtrs keyDescriptions, CharPtrs keys) {
			static const float HEADER_X = 0.4;

			static const float DESCRIPTION_COL_X = 0.25;

			// display title
			displaySectionTitle(Vector2(HEADER_X, headerY), title);

			// display key description and key columns
			displayColumn(Vector2(DESCRIPTION_COL_X, headerY - HEADER_TO_FIRST_ROW_VERTICAL_SPACE), std::move(keyDescriptions), VERTICAL_COL_SPACE, KEY_DESCRIPTION_COLOR);
			displayColumn(Vector2(DESCRIPTION_COL_X + HORIZONTAL_DESCRIPTION_KEY_COLS_SPACE, headerY - HEADER_TO_FIRST_ROW_VERTICAL_SPACE), std::move(keys), VERTICAL_COL_SPACE, KEY_COLOR);
		}

		void light() {
			displaySection(0.65, "Light", { "»Toggle TCP spotlight", "»Toggle sunlight" }, { "i", "o" });
		}

		void generic() {
			displaySection(0.35, "Generic", { "»Quit program", "»Toggle full screen" }, { "q", "p" });
		}

		void display() {
			displaySection(0.05, "Display", { "»Toggle fps", "»Toggle global coordinate system", "»Toggle axis states" }, { "b", "n", "m" });
		}

		void graphics() {
			displaySection(-0.3, "Graphics", { "»Toggle wireframe mode", "»Toggle light mode", "»Toggle backfaceculling", "»Toggle shading mode" }, { "y", "x", "c", "v" });
		}
	}
}


void displayHelp() {
	drawBackground();

	Text::displayWithShadow(Vector2(-0.05, 0.8f), "KUKA KR 16", Color::fromUnnormalizedValues(230, 80, 21), 0.003f, GLUT_BITMAP_HELVETICA_18);

	Section::LeftHalf::Robot::control();
	Section::LeftHalf::Robot::functionality();
	Section::LeftHalf::camera();

	Section::RightHalf::light();
	Section::RightHalf::generic();
	Section::RightHalf::display();
	Section::RightHalf::graphics();
}
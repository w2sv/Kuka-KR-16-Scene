#include "glutils.h"


////////////////////////////////////////////////////////////
/// Color
////////////////////////////////////////////////////////////

Color::Color(GLfloat r, GLfloat g, GLfloat b):
	r(r),
	g(g),
	b(b)
{}


Color Color::fromUnnormalizedValues(unsigned int r, unsigned int g, unsigned int b) {
	return Color(
		(float)r / 255.,
		(float)g / 255.,
		(float)b / 255.
	);
}


Color::Color(GLfloat rgb):
	r(rgb),
	g(rgb),
	b(rgb)
{}


void Color::render(bool materialized) const {
	glColor3f(r, g, b);

	if (materialized) {
		GLfloat amb_diff[4] = { r, g, b, 1 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, amb_diff);
	}
}


namespace COLORS {
	const Color BLACK(.1f);
	const Color GREY(.3f);
	const Color WHITE(1.f);
}


////////////////////////////////////////////////////////////
/// Transformation
////////////////////////////////////////////////////////////

namespace glTransformationAxes {
	namespace X {
		void rotate(float angle) {
			glRotatef(angle, 1, 0, 0);
		}
		void translate(float distance) {
			glTranslatef(distance, 0, 0);
		}
	}

	namespace Y {
		void rotate(float angle) {
			glRotatef(angle, 0, 0, 1);
		}
		void translate(float distance) {
			glTranslatef(0, 0, distance);
		}
	}

	namespace Z {
		void rotate(float angle) {
			glRotatef(angle, 0, 1, 0);
		}
		void translate(float distance) {
			glTranslatef(0, distance, 0);
		}
	}
}


void glTranslateByVec(const Vector3& vector) {
	glTranslatef(vector.x, vector.y, vector.z);
}


void glScaleUniformly(float value) { 
	glScalef(value, value, value); 
}


////////////////////////////////////////////////////////////
/// Miscellanous
////////////////////////////////////////////////////////////

void drawCoordSystem(const Extrema& x, const Extrema& y, const Extrema& z, float coneScale) {
	GLfloat i;
	GLfloat akt_color[4];
	GLint akt_mode;
	GLboolean cull_mode;

	glGetBooleanv(GL_CULL_FACE, &cull_mode);
	glDisable(GL_CULL_FACE);

	GLUquadricObj* spitze = gluNewQuadric();
	if (!spitze)
		return; // quadric konnte nicht erzeugt werden

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glGetFloatv(GL_CURRENT_COLOR, akt_color);
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(x.min, 0, 0);
		glVertex3f(x.max, 0, 0);
		for (i = x.min; i <= x.max; i++){
			glVertex3f(i, -0.15, 0.0);
			glVertex3f(i, 0.15, 0.0);
		}
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0, y.min, 0);
		glVertex3f(0, y.max, 0);

		for (i = y.min; i <= y.max; i++){
			glVertex3f(-0.15, i, 0.0);
			glVertex3f(0.15, i, 0.0);
		}

		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0, 0, z.min);
		glVertex3f(0, 0, z.max);

		for (i = z.min; i <= z.max; i++){
			glVertex3f(-0.15, 0.0, i);
			glVertex3f(0.15, 0.0, i);
		}
	glEnd();

	// Ende Linienpaare
	glGetIntegerv(GL_MATRIX_MODE, &akt_mode);
	glMatrixMode(GL_MODELVIEW);

	// zuerst die X-Achse
	glPushMatrix();
		glTranslatef(x.max, 0., 0.);
		glScaleUniformly(coneScale);
		glRotatef(90., 0., 1., 0.);
		glColor3f(1.0, 0.0, 0.0);
		gluCylinder(spitze, 0.5, 0., 1., 10, 10);
	glPopMatrix();

	// dann die Y-Achse
	glPushMatrix();
		glTranslatef(0., y.max, 0.);
		glScaleUniformly(coneScale);
		glRotatef(-90., 1., 0., 0.);
		glColor3f(0.0, 1.0, 0.0);
		gluCylinder(spitze, 0.5, 0., 1., 10, 10);
	glPopMatrix();

	// zum Schluss die Z-Achse
	glPushMatrix();
		glTranslatef(0., 0., z.max);
		glScaleUniformly(coneScale);
		glColor3f(0.0, 0.0, 1.0);
		gluCylinder(spitze, 0.5, 0., 1., 10, 10);
	glPopMatrix();

	glMatrixMode(akt_mode);
	glColor4fv(akt_color);
	glPopAttrib();
	gluDeleteQuadric(spitze);

	if (!cull_mode)
		glDisable(GL_CULL_FACE);
}
#include "glutils.h"


#pragma region Vector3
Vector3::Vector3(GLfloat x, GLfloat y, GLfloat z) {
	this->x = x;
	this->y = y;
	this->z = z;
}


std::ostream& operator<<(std::ostream& os, const Vector3& vec)
{
	os << vec.x << '/' << vec.y << '/' << vec.z << std::endl;
	return os;
}
#pragma endregion



#pragma region Vector2
Vector2::Vector2(GLdouble x, GLdouble y) {
	this->x = x;
	this->y = y;
}
#pragma endregion



#pragma region Color
Color::Color(double r, double g, double b) {
	this->r = r;
	this->g = g;
	this->b = b;
}


void Color::render() const {
	glColor3f(r, g, b);
	GLfloat amb_diff[4] = { r, g, b, 1};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, amb_diff);
}


namespace Colors {
	const Color BLACK = { .1, .1, .1 };
	const Color GREY = { .3, .3, .3 };
	const Color WHITE = { 1., 1., 1. };
}
#pragma endregion



#pragma region Rotation

void glRotatep(float angle, const GLfloat* axis) {
	glRotatef(angle, *(axis), *(axis + 1), *(axis + 2));
}

GLfloat const Axes::X[3] = { 1., 0., 0. };
GLfloat const Axes::Y[3] = { 0., 0., 1. };
GLfloat const Axes::Z[3] = { 0., 1., 0. };
#pragma endregion







void glTranslateZ(GLfloat value) { glTranslatef(0, value, 0); }


void glScaleUniformly(float value) { glScalef(value, value, value); }
#include "utils.h"


#pragma region Vector3
Vector3::Vector3(GLfloat x, GLfloat y, GLfloat z) {
	this->x = x;
	this->y = y;
	this->z = z;
}
#pragma endregion

#pragma region Color
Color::Color(GLfloat r, GLfloat g, GLfloat b, GLfloat o) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->o = o;
}

const void Color::render() {
	glColor4f(r, g, b, o);
}
#pragma endregion

#pragma region CoordinateExtrema
CoordinateExtrema::CoordinateExtrema(float min, float max) {
	// static_assert min < max;

	this->min = min;
	this->max = max;
}

CoordinateExtrema::CoordinateExtrema(float absValue) {
	CoordinateExtrema(-absValue, absValue);
}

#pragma endregion
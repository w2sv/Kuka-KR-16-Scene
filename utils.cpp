#include "utils.h"


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
Color::Color(double r, double g, double b, double a) {
	this->r = r;
	this->g = g;
	this->b = b;

	this->a = a;
}


Color::Color(int r, int g, int b, int a) {
	static float DIVISOR = 255.;

	this->r = r / DIVISOR;
	this->g = g / DIVISOR;
	this->b = b / DIVISOR;
	this->a = a / DIVISOR;
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



#pragma region CoordinateExtrema
CoordinateExtrema::CoordinateExtrema(float min, float max) {
	assert(min < max);

	this->min = min;
	this->max = max;
}


CoordinateExtrema::CoordinateExtrema(float absValue) {
	this->min = -absValue;
	this->max = absValue;
}


float CoordinateExtrema::spread() const {
	return abs(this->max - this->min);
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



# pragma region Measurements
Measurements::Measurements(float height, float width, float depth):
	height(height),
	width(width),
	depth(depth)
{}
#pragma endregion



void glTranslateZ(GLfloat value) { glTranslatef(0, value, 0); }


char* concatenatedCharPtr(const char* a, const char* b) {
	// Reference: https://stackoverflow.com/a/1995057

	char* result = (char*)calloc(strlen(a) + strlen(b) + 1, sizeof(char));
	strcpy(result, a);
	strcat(result, b);

	return result;
}


float toRadian(float degrees) {
	return M_PI / 180. * degrees;
}


std::vector<Vector2> discrete2DCircleRadiusPoints(float radius, int nPoints) {
	std::vector<Vector2> circlePoints;

	for (double t = 0; t < 2 * M_PI; t += 2 * M_PI / nPoints)
		circlePoints.push_back(Vector2(cos(t) * radius, sin(t) * radius));

	return circlePoints;
}
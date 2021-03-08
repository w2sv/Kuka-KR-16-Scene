#include "glutils.h"


#pragma region Vector
Vector3::Vector3(GLfloat x, GLfloat y, GLfloat z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3 Vector3::inverted() const {
	return Vector3(-x, -y, z);
}


std::ostream& operator<<(std::ostream& os, const Vector3& vec)
{
	os << vec.x << '/' << vec.y << '/' << vec.z << std::endl;
	return os;
}


void Vector3::glTranslate() const {
	glTranslatef(x, y, z);
}


Vector2::Vector2(GLdouble x, GLdouble y) {
	this->x = x;
	this->y = y;
}
#pragma endregion



#pragma region Color
Color::Color(double r, double g, double b) {
	if (r > 1 || g > 1 || b > 1) {
		r /= 255.;
		g /= 255.;
		b /= 255.;
	}

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


#pragma region Transformation
namespace TransformationAxes {
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



void glScaleUniformly(float value) { 
	glScalef(value, value, value); 
}
#pragma endregion

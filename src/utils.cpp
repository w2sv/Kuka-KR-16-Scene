#include "utils.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <direct.h>
#include <string>
#include <random>
#include <limits>


////////////////////////////////////////////////////////////
/// File System
////////////////////////////////////////////////////////////

char* concatenatedCharPtr(const char* a, const char* b) {
	// Reference: https://stackoverflow.com/a/1995057

	char* result = (char*)calloc(strlen(a) + strlen(b) + 1, sizeof(char));
	strcpy(result, a);
	strcat(result, b);

	return result;
}


char* joinPath(const char* a, const char* b) {
	return concatenatedCharPtr(concatenatedCharPtr(a, "\\"), b);
}


char* getResourceSubDirPath(char* subDirTitle){
	static const char* TARGET_WKDIR_TAIL = "cgpraxis-ws20-21";

	char* subDirPath = joinPath("resources", subDirTitle);
	std::string workDir(_getcwd(NULL, 0));

	if (workDir.substr(workDir.size() - strlen(TARGET_WKDIR_TAIL)) != TARGET_WKDIR_TAIL)
		return concatenatedCharPtr("..\\", subDirPath);
	return subDirPath;
}


////////////////////////////////////////////////////////////
/// Math
////////////////////////////////////////////////////////////

float toRadian(float degrees) {
	return M_PI / 180. * degrees;
}


////////////////////////////////////////////////////////////
/// Spatial
////////////////////////////////////////////////////////////

Extrema::Extrema(float min, float max) :
	min(min),
	max(max)
{}


float Extrema::spread() const {
	return abs(max - min);
}


float Extrema::clippedValue(float value) const {
	return std::max<float>(std::min<float>(max, value), min);
}


Measurements::Measurements(float height, float width, float depth) :
	height(height),
	width(width),
	depth(depth)
{}


Vector2::Vector2(float x, float y) :
	x(x), y(y)
{}


Vector2::Vector2() :
	x(NULL), y(NULL)
{}


void Vector2::set(float x, float y) {
	this->x = x;
	this->y = y;
}


void Vector2::clear() {
	x = NULL;
	y = NULL;
}


bool Vector2::isEmpty() const {
	return x == NULL && y == NULL;
}


#if DEBUG
std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
	os << vec.x << '/' << vec.y << '/' << std::endl;
	return os;
}
#endif


Vector3::Vector3(float x, float y, float z) :
	x(x),
	y(y),
	z(z)
{}


Vector3 Vector3::inverted() const {
	return Vector3(-x, -y, z);
}


#if DEBUG
std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
	os << vec.x << '/' << vec.y << '/' << vec.z << std::endl;
	return os;
}
#endif


////////////////////////////////////////////////////////////
/// Random
////////////////////////////////////////////////////////////

int randInt(const Extrema& bounds) {
	// Reference: https://stackoverflow.com/a/7560564

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(bounds.min, bounds.max);

	return distr(gen);
}
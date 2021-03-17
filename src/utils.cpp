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


std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
	os << vec.x << '/' << vec.y << '/' << std::endl;
	return os;
}


Vector3::Vector3(float x, float y, float z) :
	x(x),
	y(y),
	z(z)
{}


Vector3::Vector3():
	x(NULL),
	y(NULL),
	z(NULL)
{}


Vector3 Vector3::fromArray(std::array<GLfloat, 3> arr){
	return Vector3(arr[0], arr[1], arr[2]);
}


Vector3 Vector3::inverted() const {
	return Vector3(-x, -y, z);
}


const float* Vector3::data() const {
	return &x;
}


Vector3 Vector3::unit() const {
	return (*this) / (this->norm() + 1e-6);
}


Vector3 Vector3::operator-(const Vector3& other) const {
	return Vector3(
		x - other.x,
		y - other.y,
		z - other.z
	);
}


Vector3 Vector3::operator/(float divisor) const {
	return Vector3(
		x / divisor,
		y / divisor,
		z / divisor
	);
}



float Vector3::norm() const {
	return sqrt(x * x + y * y + z * z);
}


std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
	os << vec.x << '/' << vec.y << '/' << vec.z;
	return os;
}


Vector3 crossProduct(const Vector3& a, const Vector3& b) {
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}


Vector3 normalVector(const Vector3& a, const Vector3& b, const Vector3& c) {
	return crossProduct(b - a, b - c).unit();
}


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
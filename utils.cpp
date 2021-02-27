#include "utils.h"


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


#pragma region CoordinateExtrema
Extrema::Extrema(float min, float max) {
	assert(min < max);

	this->min = min;
	this->max = max;
}


float Extrema::spread() const {
	return abs(this->max - this->min);
}
#pragma endregion


Measurements::Measurements(float height, float width, float depth) :
	height(height),
	width(width),
	depth(depth)
{}

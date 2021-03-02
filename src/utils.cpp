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


bool toggleFlag(bool flag) {
	return (bool)(-(int)flag + 1);
}


int randInt(const Extrema& bounds) {
	// Reference: https://stackoverflow.com/a/7560564

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(bounds.min, bounds.max);

	return distr(gen);
}
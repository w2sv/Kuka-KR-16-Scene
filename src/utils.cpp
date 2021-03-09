#include "utils.h"


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

Extrema::Extrema(float min, float max) {
	assert(min < max);

	this->min = min;
	this->max = max;
}



float Extrema::spread() const {
	return abs(this->max - this->min);
}



Measurements::Measurements(float height, float width, float depth) :
	height(height),
	width(width),
	depth(depth)
{}



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
#pragma once

#include "../dependencies/freeglut.h"

#include <iostream>
#include <array>
#include <vector>
#include <time.h>


/*
* Class allowing for abstraction of numeric value by enabling object usage in conjunction with
* entirety of comparison, as well as arithmetic operators
*/
template <typename T> struct ValueAbstraction {
	T value;
	ValueAbstraction(T value): value(value) {}
	
	bool operator<(const ValueAbstraction& other) const { return value < other.value; }
	bool operator>(const ValueAbstraction& other) const { return value > other.value; }
	bool operator<=(const ValueAbstraction& other) const { return value <= other.value; }
	bool operator>=(const ValueAbstraction& other) const { return value >= other.value; }
	bool operator==(const ValueAbstraction& other) const { return value == other.value; }
	bool operator!=(const ValueAbstraction& other) const { return value != other.value; }
	
	void operator+=(const ValueAbstraction<T>& other) { value += other.value; }
	void operator-=(const ValueAbstraction<T>& other) { value -= other.value; }
	friend T operator+(const ValueAbstraction<T>& inst, const ValueAbstraction<T>& other) { return inst.value + other.value; }
	friend T operator-(const ValueAbstraction<T>& inst, const ValueAbstraction<T>& other) { return inst.value - other.value; }

	void operator*=(const ValueAbstraction& other) { inst.value *= other.value; }
	void operator/=(const ValueAbstraction& other) { inst.value /= other.value; }
	friend T operator*(const ValueAbstraction<T>& inst, const ValueAbstraction<T>& other) { return inst.value * other.value; }
	friend T operator/(const ValueAbstraction<T>& inst, const ValueAbstraction<T>& other) { return inst.value / other.value; }
};

////////////////////////////////////////////////////////////
/// File System
////////////////////////////////////////////////////////////

typedef const char* CharPtr;
typedef std::vector<CharPtr> CharPtrVec;

/*
* Abstraction of file path collection the elements of which are
* accessible via [size_t] and assuring respective base paths to be set to repo root
*/
struct AbsolutePaths {
	AbsolutePaths(CharPtr relativeDirPath, CharPtrVec fileNames);
	CharPtr operator[](size_t index) const;

	size_t size() const;
private:
	CharPtrVec absolutePaths;
};

/*
* Assures base level of created relative file path to be set to repository root
* 
* @return rootSubDirPath if current workDir == repo root, otherwise ..\\{rootSubDirPath}
*/
char* repoRootSubDirPath(CharPtr rootSubDirPath);

/// Concatenates passed path components whilst adding \\ in between 
char* joinPath(CharPtr a, CharPtr b);

char* concatenatedCharPtr(CharPtr a, CharPtr b);

////////////////////////////////////////////////////////////
/// Math
////////////////////////////////////////////////////////////

float toRadian(float degrees);

////////////////////////////////////////////////////////////
/// Spatial
////////////////////////////////////////////////////////////

struct Extrema {
	float min, max;
	Extrema(float min, float max);

	float spread() const;
	float clippedValue(float value) const;
};

struct Vector2 {
	float x, y;
	Vector2(float x, float y);
	Vector2();

	void set(float x, float y);
	void clear();
	bool isEmpty() const;
	friend std::ostream& operator<<(std::ostream& os, const Vector2& vector);
};

struct Vector3 {
	float x, y, z;
	Vector3(float x, float y, float z);
	Vector3();
	static Vector3 fromArray(std::array<GLfloat, 3> arr);

	const float* data() const;
	Vector3 inverted() const;
	float norm() const;
	Vector3 unit() const;

	Vector3 operator-(const Vector3& other) const;
	Vector3 operator/(float divisor) const;
	friend std::ostream& operator<<(std::ostream& os, const Vector3& vector);
};

Vector3 crossProduct(const Vector3& a, const Vector3& b);
Vector3 normalVector(const Vector3& a, const Vector3& b, const Vector3& c);

////////////////////////////////////////////////////////////
/// Random
////////////////////////////////////////////////////////////

int randInt(const Extrema& bounds);

////////////////////////////////////////////////////////////
/// Time
////////////////////////////////////////////////////////////

time_t& currentSecondsTimestamp();
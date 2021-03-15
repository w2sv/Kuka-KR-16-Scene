#pragma once


////////////////////////////////////////////////////////////
/// ValueAbstraction Baseclass
////////////////////////////////////////////////////////////

template <typename T> struct ValueAbstraction {
	T value;
	ValueAbstraction(T value): value(value) {}
	~ValueAbstraction(){}
	
	bool operator<(const ValueAbstraction& other) { return value < other.value; }
	bool operator>(const ValueAbstraction& other) { return value > other.value; }
	bool operator<=(const ValueAbstraction& other) { return value <= other.value; }
	bool operator>=(const ValueAbstraction& other) { return value >= other.value; }
	bool operator==(const ValueAbstraction& other) { return value == other.value; }
	bool operator!=(const ValueAbstraction& other) { return value != other.value; }
	
	void operator+=(const ValueAbstraction<T>& other) { value += other.value; }
	void operator-=(const ValueAbstraction<T>& other) { value -= other.value; }
	friend T operator+(const ValueAbstraction<T>& inst, const ValueAbstraction<T>& other) { return inst.value + other.value; }
	friend T operator-(const ValueAbstraction<T>& inst, const ValueAbstraction<T>& other) { return inst.value - other.value; }

	void operator*=(const ValueAbstraction& other) { inst.value *= other.value; }
	void operator/=(const ValueAbstraction& other) { inst.value /= other.value; }
	friend T operator*(const ValueAbstraction<T>& inst, const ValueAbstraction<T>& other) { return inst.value * other.value; }
	friend T operator/(const ValueAbstraction<T>& inst, const ValueAbstraction<T>& other) { return inst.value / other.value; }
#if DEBUG
	virtual friend std::ostream& operator<<(std::ostream& os, const ValueAbstraction<T>& inst) { os << value; return os; }
#endif
};

////////////////////////////////////////////////////////////
/// File System
////////////////////////////////////////////////////////////

char* concatenatedCharPtr(const char* a, const char* b);

/// Concatenates passed path components whilst adding \\ in between 
char* joinPath(const char* a, const char* b);

/// Assures base level of created relative file path to be set to repository root
char* getResourceSubDirPath(char* subDirTitle);

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
};

struct Measurements {
	float height, width, depth;
	Measurements(float height, float width, float depth);
};

struct Vector2 {
	float x, y;
	Vector2(float x, float y);
	Vector2();

	void set(float x, float y);
	void clear();
	bool isEmpty() const;
#if DEBUG
	friend std::ostream& operator<<(std::ostream& os, const Vector2& vector);
#endif
};

struct Vector3 {
	float x, y, z;
	Vector3(float x, float y, float z);

	Vector3 inverted() const;
#if DEBUG
	friend std::ostream& operator<<(std::ostream& os, const Vector3& vector);
#endif
};

////////////////////////////////////////////////////////////
/// Random
////////////////////////////////////////////////////////////

int randInt(const Extrema& bounds);
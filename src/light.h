#pragma once

#include "../dependencies/freeglut.h"

#include <array>


// Farbe setzen fuer Farb- und Beleuchtungsmodus
void setColor(GLfloat r, GLfloat g, GLfloat b);

struct Material {
	typedef std::array<GLfloat, 4> RGBAParameter;

	Material(RGBAParameter amb, RGBAParameter diff, RGBAParameter spec, GLfloat shine, RGBAParameter emis);

	void set() const;
private:
	RGBAParameter amb;
	RGBAParameter diff;
	RGBAParameter spec;
	GLfloat shine;
	RGBAParameter emis;
};

// Lichtquelle(n) setzen
void setLight();

// Standardeinstellung für Farbe und Beleuchtung
void setDefaultLightAndMaterial(GLboolean lightMode = false);
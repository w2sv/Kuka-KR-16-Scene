#pragma once

#include "utils.h"

#include "../dependencies/freeglut.h"

#include <array>


typedef std::array<GLfloat, 4> RGBAParameter;

struct Material {
	Material(RGBAParameter amb, RGBAParameter diff, RGBAParameter spec, GLfloat shine, RGBAParameter emis);

	void set() const;
private:
	RGBAParameter amb;
	RGBAParameter diff;
	RGBAParameter spec;
	GLfloat shine;
	RGBAParameter emis;
};

void setLight();

// Standardeinstellung für Farbe und Beleuchtung
void setDefaultLightAndMaterial(GLboolean lightMode);

typedef std::array<GLfloat, 3> XYZParameter;

class cg_light{
public:
	// Konstructior, id zwischen 0-7 muss uebergeben werden
	cg_light(int num, RGBAParameter position, RGBAParameter amb, RGBAParameter diff, RGBAParameter spec, XYZParameter attenuation = XYZParameter{ 1, 0, 0 }, XYZParameter spotDirection = XYZParameter{ 0, 0, 0 }, float spotCutoff = 180, float spotExp = 0);

	void draw() const;
	void disable() const;

	void markLightPosition();

	XYZParameter spotDir;

private:
	const int id;
	
	const RGBAParameter pos;

	const RGBAParameter amb;
	const RGBAParameter diff;
	const RGBAParameter spec;
	
	XYZParameter att;
	float spotCutoff;
	float spotExp;
};
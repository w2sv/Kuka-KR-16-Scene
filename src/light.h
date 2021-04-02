#pragma once

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

void drawSunlight();

// Standardeinstellung für Farbe und Beleuchtung
void setDefaultLightAndMaterial(GLboolean lightMode = false);

class cg_light{
public:
	// Konstructior, id zwischen 0-7 muss uebergeben werden
	cg_light(int num);

	// wenn w=0 -> direktionales Licht, Richtung wird eingespeichert
	// wenn w=1 -> Punktlichtquelle, Positon wird eingespeichert
	void setPosition(float x, float y, float z, float w);

	// ambienten Anteil der Lichtfarbe einspeichern
	void setAmbient(float r, float g, float b, float a);
	// diffusen Anteil der Lichtfarbe einspeichern
	void setDiffuse(float r, float g, float b, float a);
	// specularen Anteil der Lichtfarbe einspeichern
	void setSpecular(float r, float g, float b, float a);

	// Richtung, Oeffnungswinkel und exponent einspeichern (nur bei Punktlichtquelle sinnvoll)
	// cutoff = [0..90], 180
	// exponent = [0..128]
	void setSpotlight(float directionX, float directionY, float directionZ, float cutoff, float exponent);

	// Abschwächungsfaktoren der Lichtintensität einspeichern
	void setAttentuation(float constant, float linear, float quadric);

	// Licht mit den bisher eingespeicherten Parametern rendern (falls es eingeschaltet ist)
	void draw();

	void disable() const;

	// eine kleine Kugel in der Farbe des diffusen Lichtanteils an die Position der Lichtquelle zeichnen
	void markLightPosition();

	float pos[4];
	float rot[2];
	float spot[5];

private:
	int id;
	float amb[4];
	float diff[4];
	float spec[4];
	float att[3];
};

static cg_light sunlight(1);
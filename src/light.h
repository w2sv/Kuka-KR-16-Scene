#pragma once

#include "../dependencies/freeglut.h"


// Farbe setzen fuer Farb- und Beleuchtungsmodus
void setColor(GLfloat r, GLfloat g, GLfloat b);

// Material setzen (nur fuer Beleuchtungsmodus)
void setMaterial(GLenum face, GLfloat amb[4], GLfloat diff[4], GLfloat spec[4], GLfloat shine, GLfloat emis[4]);

// Lichtquelle(n) setzen
void setLights();

// Standardeinstellung für Farbe und Beleuchtung
void setDefaultLightAndMaterial(GLboolean lightMode = false);
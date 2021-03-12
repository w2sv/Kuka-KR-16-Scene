#pragma once

#include "utils.h"
#include "glutils.h"

#include "../dependencies/freeglut.h"

#include <vector>
#include <array>


void drawCube();

void drawCylinder(float startRadius, float endRadius, float height);

namespace OctogonalPrism {
	typedef std::array<std::array<std::array<GLfloat, 3>, 8>, 2> Vertices;	
	Vertices draw(float heigth, float straightEdgeLength, float diagonalEdgeLength);
	
	void drawCage(Vertices vertices);
}

void drawQuadraticGrid(const Extrema& limits, int tiles, Color& color);

void drawPlane(const Extrema& xExtrema, const Extrema& yExtrema);

////////////////////////////////////////////////////////////
/// Dev functions
////////////////////////////////////////////////////////////

#ifdef DEBUG
void indicateCurrentPosition();

void drawZVector();
#endif
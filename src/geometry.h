#pragma once

#include <vector>

#include "../dependencies/freeglut.h"

#include "utils.h"
#include "glutils.h"


void drawCube();

void drawCylinder(float startRadius, float endRadius, float height);

namespace OctogonalPrism {
	typedef std::vector<std::vector<std::vector<GLfloat>>> Vertices;  // of shape [2][8][3]  TODO: incorporate dimensionalities into type via std::array
	
	Vertices draw(float heigth, float straightEdgeLength, float diagonalEdgeLength);
	
	void drawCage(Vertices vertices);
}

void drawQuadraticGrid(const Extrema& xExtrema, int tiles, Color& color);

void drawPlane(const Extrema& xExtrema, const Extrema& yExtrema);

////////////////////////////////////////////////////////////
/// Dev functions
////////////////////////////////////////////////////////////

#ifdef DEBUG
void indicateCurrentPosition();

void drawZVector();
#endif
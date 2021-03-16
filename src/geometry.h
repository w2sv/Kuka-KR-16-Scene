#pragma once

#include "utils.h"
#include "glutils.h"

#include "../dependencies/freeglut.h"

#include <array>


void drawCube();

void drawCylinder(float startRadius, float endRadius, float height);

namespace OctogonalPrism {
	typedef std::array<GLfloat, 3> Vertex;
	typedef std::array<std::array<Vertex, 8>, 2> Vertices;	
	Vertices draw(float heigth, float straightEdgeLength, float diagonalEdgeLength);
	void drawCage(Vertices vertices);
	void drawWithCage(float height, float straightEdgeLength, float diagonalEdgeLength, const Color& corpusColor, const Color& cageColor);
}

void drawQuadraticGrid(const Extrema& limits, int tiles, const Color& color);

void drawPlane(const Extrema& xExtrema, const Extrema& yExtrema);

////////////////////////////////////////////////////////////
/// Dev functions
////////////////////////////////////////////////////////////

#ifdef DEBUG
void indicateCurrentPosition();

void drawZVector();
#endif
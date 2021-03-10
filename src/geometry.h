#pragma once

#include <cassert>
#include <vector>

#include "../dependencies/freeglut.h"

#include "glutils.h"
#include "utils.h"
#include "light.h"



void drawCube();
void drawQuadraticGrid(const Extrema& xExtrema, int tiles, Color& color);
void drawPlane(const Extrema& xExtrema, const Extrema& yExtrema);
void drawCylinder(float startRadius, float endRadius, float height);

// --------------------------OctogonalPrism---------------------------

typedef std::vector<std::vector<std::vector<GLfloat>>> OctagonalPrismVertices;  // of shape [2][8][3]  TODO: incorporate dimensionalities into type via std::array
OctagonalPrismVertices drawOctagonalPrism(float heigth, float straightEdgeLength, float diagonalEdgeLength);
void drawOctagonalPrismCage(OctagonalPrismVertices);

// -----------Dev functions, to be removed after dev termination-----------

void indicateCurrentPosition();
void drawZVector();
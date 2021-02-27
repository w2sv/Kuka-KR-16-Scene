#pragma once

#include <cassert>
#include <vector>

#include "Include/freeglut.h"

#include "utils.h"
#include "light.h"


const Color EDGE_COLOR(.1, .1, .1);


void drawCube();

// dev function, to be removed after dev termination
void indicateCurrentPosition();

void drawCuboid(float length, float height, float depth);

void drawQuadraticGrid(Extrema& xExtrema, int tiles, Color& color);

void drawPlane(Extrema& xExtrema, Extrema& yExtrema, Color& color);

void drawCylinder(float startRadius, float endRadius, float height);


#pragma region OctagonalPrism
typedef std::vector<std::vector<std::vector<GLfloat>>> OctagonalPrismVertices;  // of shape [2][8][3]  TODO: incorporate dimensionalities into type via std::array

OctagonalPrismVertices drawOctagonalPrism(float heigth, float straightEdgeLength, float diagonalEdgeLength);

void drawOctagonalPrismCage(OctagonalPrismVertices);
#pragma endregion
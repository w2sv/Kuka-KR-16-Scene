#pragma once

#include <cassert>
#include <vector>
#include <array>

#include "Include/freeglut.h"

#include "utils.h"
#include "light.h"


const Color EDGE_COLOR(.1, .1, .1);


void drawCube();

void drawCuboid(float length, float height, float depth);

void drawQuadraticGrid(CoordinateExtrema& xExtrema, int tiles, Color& color);

void drawPlane(CoordinateExtrema& xExtrema, CoordinateExtrema& yExtrema, Color& color);

void drawCylinder(float startRadius, float endRadius, float height);


#pragma region Octagon
typedef std::vector<std::vector<std::vector<GLfloat>>> OctagonVertices;  // of shape [2][8][3]  TODO: incorporate dimensionalities into type via std::array

OctagonVertices drawOctagon(float heigth, float straightEdgeLength, float diagonalEdgeLength);

void drawOctagonCage(OctagonVertices);
#pragma endregion
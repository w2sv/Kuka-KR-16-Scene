#pragma once

#include <cassert>

#include "Include/freeglut.h"

#include "utils.h"
#include "light.h"


const Color EDGE_COLOR(1., 0.1, 0.1);


void drawCube();

void drawCuboid(float length, float height, float depth);

void drawQuadraticGrid(CoordinateExtrema& xExtrema, int tiles, Color& color);

void drawPlane(CoordinateExtrema& xExtrema, CoordinateExtrema& yExtrema, Color& color);

void drawCylinder(float startRadius, float endRadius, float height);

void drawOctagon(float heigth, float edgeLength, bool emphasizeEdges = true);

void _drawOctagonEdges(GLfloat octagonVertices[2][8][3]);
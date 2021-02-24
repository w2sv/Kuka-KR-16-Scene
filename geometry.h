#pragma once

#include <cassert>

#include "Include/freeglut.h"

#include "utils.h"
#include "light.h"

void drawCube();

void drawCuboid(float length, float height, float depth);

void drawQuadraticGrid(CoordinateExtrema& xExtrema, int tiles, Color& color);

void drawPlane(CoordinateExtrema& xExtrema, CoordinateExtrema& yExtrema, Color& color);

void drawCylinder(float startRadius, float endRadius, float height);
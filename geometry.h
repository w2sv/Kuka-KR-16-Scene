#pragma once

#include <cassert>

#include "Include/freeglut.h"

#include "utils.h"

void drawCuboid(float length, float height, float depth, Color& color);

void drawQuadraticGrid(CoordinateExtrema& xExtrema, int tiles, Color& color);

void drawPlane(CoordinateExtrema& xExtrema, CoordinateExtrema& yExtrema, Color& color);

void drawCylinder(float startRadius, float endRadius, float height);

void drawEllipsoid();
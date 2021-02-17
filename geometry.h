#pragma once

#include "Include/freeglut.h"

#include "utils.h"

void drawCuboid(float length, float height, float depth, Color &color);

void drawGrid(CoordinateExtrema& xExtrema, CoordinateExtrema& yExtrema, Color& color);

void drawPlane(CoordinateExtrema& xExtrema, CoordinateExtrema& yExtrema, Color& color);
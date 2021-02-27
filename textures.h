#pragma once

#include <iostream>

#include "Include/freeglut.h"

#include "utils.h"
#include "image.h"


#define NUM_TEXTURES 1


cg_image textures[NUM_TEXTURES];


void initTextures();

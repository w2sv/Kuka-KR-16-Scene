#include "skybox.h"

#include "geometry.h"

#include "../dependencies/freeglut.h"
#include "../dependencies/glext.h"

#include <array>


CubeMap cubeMap;


void Skybox::loadTextures() {
	const char* DIR_PATH = getResourceSubDirPath("textures\\skybox");
	const char* FILE_NAMES[CubeMap::N_FACES] = {
		"right.bmp",
		"left.bmp",
		"top.bmp",
		"bottom.bmp",
		"back.bmp",
		"front.bmp"
	};

	CubeMap::SideFilePaths sideFilePaths;
	for (size_t i = 0; i < CubeMap::N_FACES; i++)
		sideFilePaths[i] = joinPath(DIR_PATH, FILE_NAMES[i]);

	cubeMap.load(sideFilePaths, true);
}


/* 
* Strips any sort of zooming-based translation off of modelview matrix, making environment 
* depicted by skybox thus appear tremendously far away
 */
void enableParallaxeEffect() {
	static const size_t TRANSLATE_X_INDEX = 12;
	
	static GLfloat modelViewMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);

	for (size_t i = 0; i < 3; i++)
		modelViewMatrix[TRANSLATE_X_INDEX + i] = 0.f;

	glLoadMatrixf(modelViewMatrix);
}


void Skybox::draw() {
	glPushMatrix();
		enableParallaxeEffect();
		glScaleUniformly(64);
	
		glDepthMask(GL_FALSE);
		glEnable(GL_TEXTURE_CUBE_MAP);	
			cubeMap.bind();
			Cube::draw(true);
		glDisable(GL_TEXTURE_CUBE_MAP);
		glDepthMask(GL_TRUE);
	glPopMatrix();
}
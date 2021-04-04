#include "skybox.h"

#include "geometry.h"
#include "image.h"
#include "utils.h"

#include "../dependencies/freeglut.h"
#include "../dependencies/glext.h"


CubeMap cubeMap;


void Skybox::loadTextures() {
	cubeMap.load(
		AbsolutePaths(
			"resources\\textures\\skybox", {
				"right.bmp",
				"left.bmp",
				"top.bmp",
				"bottom.bmp",
				"back.bmp",
				"front.bmp"
			}
		), false
	);
}


/* 
* Strips any sort of zooming-based translation off of modelview matrix, making environment 
* depicted by skybox thus appear tremendously far away
 */
void enableParallaxeEffect() {
	static const size_t TRANSLATE_X_INDEX = 12;
	
	// retrieve current modelview matrix
	static GLfloat modelViewMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);

	// set components of translation vector part of matrix to zero
	for (size_t i = 0; i < 3; i++)
		modelViewMatrix[TRANSLATE_X_INDEX + i] = 0.f;

	// enable manipulated modelview matrix
	glLoadMatrixf(modelViewMatrix);
}


void Skybox::draw() {
	glPushMatrix();
		enableParallaxeEffect();
		glScaleUniformly(EXTENT);
	
		glDepthMask(GL_FALSE);
		glEnable(GL_TEXTURE_CUBE_MAP);	
			cubeMap.bind();
			Cube::draw(true);
		glDisable(GL_TEXTURE_CUBE_MAP);
		glDepthMask(GL_TRUE);
	glPopMatrix();
}
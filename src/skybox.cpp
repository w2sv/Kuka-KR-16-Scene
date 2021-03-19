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

	cubeMap.load(sideFilePaths);
}


void Skybox::draw() {
	glPushMatrix();
		glScaleUniformly(400);
	
		glDepthMask(GL_FALSE);
		glEnable(GL_TEXTURE_CUBE_MAP);

			cubeMap.bind();
			Cube::draw(true);

		glDisable(GL_TEXTURE_CUBE_MAP);
		glDepthMask(GL_TRUE);
	glPopMatrix();
}
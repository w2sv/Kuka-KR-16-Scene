#include "skybox.h"

#include "geometry.h"

#include "../dependencies/freeglut.h"
#include "../dependencies/glext.h"

#include <array>


const int N_SKYBOX_TEXTURES = 6;
CubeMap cubeMap;


void Skybox::loadTextures() {
	const char* DIR_PATH = getResourceSubDirPath("textures\\skybox");
	const char* FILE_NAMES[N_SKYBOX_TEXTURES] = {
		"front.bmp",
		"back.bmp",
		"left.bmp",
		"right.bmp",
		"top.bmp",
		"bottom.bmp"
	};

	CubeMap::SideFilePaths sideFilePaths;
	for (size_t i = 0; i < 6; i++)
		sideFilePaths[i] = joinPath(DIR_PATH, FILE_NAMES[i]);

	cubeMap.load(sideFilePaths);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
}


void Skybox::draw() {
	glPushMatrix();
		glDepthMask(GL_FALSE);
		glScaleUniformly(100);
		// glTransformationAxes::Y::rotate(-90);
	
		glEnable(GL_TEXTURE_CUBE_MAP);
			/*glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);*/

			// glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			cubeMap.bind();

			for (size_t i = 0; i < 6; i++) {
				Cube::drawFace(Cube::Face(i));
			}

			/*glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);*/

		glDisable(GL_TEXTURE_CUBE_MAP);
		glDepthMask(GL_TRUE);
	glPopMatrix();
}
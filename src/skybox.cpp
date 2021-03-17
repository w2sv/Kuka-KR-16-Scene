#include "skybox.h"

#include "geometry.h"

#include "../dependencies/freeglut.h"
#include "../dependencies/glext.h"


const int N_SKYBOX_TEXTURES = 6;
static cg_image skyboxTextures[N_SKYBOX_TEXTURES];


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

	/*const GLenum sideTarget[N_SKYBOX_TEXTURES] = {
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	};*/

	for (size_t i = 0; i < N_SKYBOX_TEXTURES; i++) {
		skyboxTextures[i].load(joinPath(DIR_PATH, FILE_NAMES[i]), false);

		skyboxTextures[i].setMinFilter(GL_NEAREST);
		skyboxTextures[i].setMagFilter(GL_LINEAR);
		skyboxTextures[i].setWrapMode(GL_CLAMP);
		skyboxTextures[i].setEnvMode(GL_MODULATE);

		std::cout << skyboxTextures[i].fileName << std::endl;
	}
}


void Skybox::draw() {
	glEnable(GL_TEXTURE_2D);

	for (size_t i = 0; i < 6; i++) {
		skyboxTextures[i].bind();
		Cube::drawFace(Cube::Face(i));
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glDisable(GL_TEXTURE_2D);
}
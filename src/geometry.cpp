#include "geometry.h"

#include "../dependencies/freeglut.h"
#include "../dependencies/glext.h"

#include <limits>


using namespace glTransformationAxes;


////////////////////////////////////////////////////////////
/// Constants
////////////////////////////////////////////////////////////

const unsigned int SLICES = 20;
const unsigned int STACKS = 20;
const unsigned int LOOPS = 1;


const float PSEUDO_NULL = -0.01;
const float SQUARE_VERTEX_COORD = 0.5f;

const GLfloat SQUARE_TEXTURE_COORDINATES[4][3] = {
	{1., 0.},
	{1., 1.},
	{0., 1.},
	{0., 0.}
};


////////////////////////////////////////////////////////////
/// Pseudo 2D
////////////////////////////////////////////////////////////

void drawQuadraticGrid(const Extrema& extrema, int tiles, const Color& color) {
	color.render();

	glBegin(GL_LINES);
	for (float v = extrema.min; v <= extrema.max; v += extrema.spread() / tiles) {
		v == extrema.min || v == extrema.max ? Color(.6f, .3f, .3f).render() : color.render();
		glVertex3f(v, 0, extrema.min);
		glVertex3f(v, 0, extrema.max);

		v == extrema.min || v == extrema.max ? Color(.3f, .3f, .6f).render() : color.render();
		glVertex3f(extrema.min, 0, v);
		glVertex3f(extrema.max, 0, v);
	};
	glEnd();
}


void drawPlane(const Extrema& xExtrema, const Extrema& yExtrema) {
	glBegin(GL_QUADS);
		glVertex3f(xExtrema.min, PSEUDO_NULL, yExtrema.min);
		glVertex3f(xExtrema.max, PSEUDO_NULL, yExtrema.min);
		glVertex3f(xExtrema.max, PSEUDO_NULL, yExtrema.max);
		glVertex3f(xExtrema.min, PSEUDO_NULL, yExtrema.max);
	glEnd();
}


////////////////////////////////////////////////////////////
/// 3D
////////////////////////////////////////////////////////////

void Cube::drawFace(Face face, bool texture3D) {
	static const GLfloat VERTICES[2][4][3] = {
		{
			{SQUARE_VERTEX_COORD, SQUARE_VERTEX_COORD, SQUARE_VERTEX_COORD},
			{SQUARE_VERTEX_COORD, SQUARE_VERTEX_COORD, -SQUARE_VERTEX_COORD},
			{-SQUARE_VERTEX_COORD, SQUARE_VERTEX_COORD, -SQUARE_VERTEX_COORD},
			{-SQUARE_VERTEX_COORD, SQUARE_VERTEX_COORD, SQUARE_VERTEX_COORD}
		},
		{
			{SQUARE_VERTEX_COORD, -SQUARE_VERTEX_COORD, SQUARE_VERTEX_COORD},
			{ SQUARE_VERTEX_COORD, -SQUARE_VERTEX_COORD, -SQUARE_VERTEX_COORD },
			{ -SQUARE_VERTEX_COORD, -SQUARE_VERTEX_COORD, -SQUARE_VERTEX_COORD },
			{ -SQUARE_VERTEX_COORD, -SQUARE_VERTEX_COORD, SQUARE_VERTEX_COORD }
		}
	};

	static const GLfloat TEX_COORDS_3D[6][4][3] = {
		{{-1.0f, -1.0f, -1.0f} , {-1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f} ,{-1.0f, 1.0f, -1.0f}},
		{{1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f}},
		{{1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
		{{1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f}},
		{{1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
		{{-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}}
	};

	static const GLfloat NORMAL_VECTORS[6][3] = {
		{1, 0, 0},
		{-1, 0, 0},
		{0, 1, 0},
		{0, -1, 0},
		{0, 0, 1},
		{0, 0, -1},
	};

	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 2; j++) {
			size_t vertexIndex = i * 2 + j;

			if (texture3D)
				glTexCoord3fv(TEX_COORDS_3D[face][vertexIndex]);
			else {
				glTexCoord2fv(SQUARE_TEXTURE_COORDINATES[vertexIndex]);
				glNormal3fv(NORMAL_VECTORS[face]);
			}

			switch (face) {
				case Front:
					glVertex3fv(VERTICES[i][i != j]); break;

				case Back:
					glVertex3fv(VERTICES[i][(i != j) + 2]); break;

				case Left:
					glVertex3fv(VERTICES[i][(i != j) + 1]); break;

				case Right:
					glVertex3fv(VERTICES[i][(i != j) * 3]); break;

				case Top:
					glVertex3fv(VERTICES[0][vertexIndex]); break;

				case Bottom:
					glVertex3fv(VERTICES[1][vertexIndex]); break;
			}
		}
	}
}


void Cube::draw(bool texture3D) {
	glBegin(GL_QUADS);
		for (size_t i = 0; i < 6; i++)
			drawFace(Face(i), texture3D);
	glEnd();
}


void drawCylinder(float startRadius, float endRadius, float height) {
	GLUquadricObj* q = gluNewQuadric();

	gluQuadricNormals(q, true);
	gluQuadricTexture(q, true);

	glPushMatrix();
		X::rotate(270);

		// Mantelfläche
		gluCylinder(q, startRadius, endRadius, height, SLICES, STACKS);

		// Deckel
		glTranslatef(0, 0, height);
		gluDisk(q, 0, endRadius, SLICES, LOOPS);

		// Boden
		glTranslatef(0, 0, -height);
		glRotatef(180, 0, 1, 0);
		gluDisk(q, 0, startRadius, SLICES, LOOPS);
	glPopMatrix();

	gluDeleteQuadric(q);
}


////////////////////////////////////////////////////////////
/// OctagonalPrism
////////////////////////////////////////////////////////////

OctogonalPrism::OctogonalPrism(float heigth, float straightEdgeLength, float diagonalEdgeLength){
	const float lateralLength = (straightEdgeLength + diagonalEdgeLength) / 2;
	const float intermediateCoordinate = straightEdgeLength / 2;
	const float z = heigth / 2;
	
	vertices = calculateVertices(lateralLength, intermediateCoordinate, z);
	flatFaceTextureVertices = calculateFlatFaceTextureVertices(lateralLength, intermediateCoordinate);
}


OctogonalPrism::Vertices OctogonalPrism::calculateVertices(float lateralLength, float intermediateCoordinate, float z) {
	return Vertices{ {
		{
			/* Subsequently denoted are the vertices in clockwise direction with respect
			to the respectively annotated edge;
			i.e. "left |" defines the lower vertex of the left | etc. */

			// positive y's
			Vertex{-lateralLength, z, -intermediateCoordinate}, //  left |
			Vertex{-intermediateCoordinate, z, -lateralLength}, // lower left "\"
			Vertex{intermediateCoordinate, z, -lateralLength}, // -
			Vertex{lateralLength, z, -intermediateCoordinate}, // lower right /

			// negative y's
			Vertex{lateralLength, z, intermediateCoordinate}, // right |
			Vertex{intermediateCoordinate, z, lateralLength}, // upper right "\"
			Vertex{-intermediateCoordinate, z, lateralLength}, // -
			Vertex{-lateralLength, z, intermediateCoordinate}, // upper left /
		},

		// negative z
		{
			// positive y's
			Vertex{-lateralLength, -z, -intermediateCoordinate}, //  left |
			Vertex{-intermediateCoordinate, -z, -lateralLength}, // lower left "\"
			Vertex{intermediateCoordinate, -z, -lateralLength}, // -
			Vertex{lateralLength, -z, -intermediateCoordinate}, // lower right /

			// negative y's
			Vertex{lateralLength, -z, intermediateCoordinate}, // right |
			Vertex{intermediateCoordinate, -z, lateralLength}, // upper right "\"
			Vertex{-intermediateCoordinate, -z, lateralLength}, // -
			Vertex{-lateralLength, -z, intermediateCoordinate}, // upper left /
		}
	} };
}


OctogonalPrism::FlatFaceTextureVertices OctogonalPrism::calculateFlatFaceTextureVertices(float lateralLength, float intermediateCoordinate) {
	const float positiveNormalizedIntermediateCoordinate = 0.5 + intermediateCoordinate / lateralLength;
	const float negativeNormalizedIntermediateCoordinate = 0.5 - intermediateCoordinate / lateralLength;
	
	return FlatFaceTextureVertices{
		{
			// positive y's
			TextureVertex{0, positiveNormalizedIntermediateCoordinate}, //  left |
			TextureVertex{negativeNormalizedIntermediateCoordinate, 0}, // lower left "\"
			TextureVertex{positiveNormalizedIntermediateCoordinate, 0}, // -
			TextureVertex{1, negativeNormalizedIntermediateCoordinate}, // lower right /
	
			// negative y's
			TextureVertex{1, positiveNormalizedIntermediateCoordinate}, // right |
			TextureVertex{positiveNormalizedIntermediateCoordinate, 1}, // upper right "\"
			TextureVertex{negativeNormalizedIntermediateCoordinate, 1}, // -
			TextureVertex{0, positiveNormalizedIntermediateCoordinate}, // upper left /
		}
	};
}


OctogonalPrism::UprightFaceNormalVectors OctogonalPrism::calculateUprightFacesNormalVectors() const {
	UprightFaceNormalVectors vectors;

	for (size_t faceIndex = 0; faceIndex < 8; faceIndex++) {
		vectors[faceIndex] = normalVector(
			Vector3::fromArray(vertices[0][faceIndex]),
			Vector3::fromArray(vertices[0][(faceIndex + 1) % 8]),
			Vector3::fromArray(vertices[1][(faceIndex + 1) % 8])
		);
	}

	return vectors;
}


void OctogonalPrism::draw(const Color& corpusColor, const Color* cageColor) const {
	corpusColor.render();

	// Flat Faces
	{
		const static GLdouble NORMAL_VECTORS[2][3] = {
			{0, -1, 0},
			{0, 1, 0}
		};

		//draw upper and lower plane
		for (size_t zPolarityIndex = 0; zPolarityIndex < 2; zPolarityIndex++) {
			glBegin(GL_POLYGON);
			glNormal3dv(NORMAL_VECTORS[zPolarityIndex]);
			for (size_t i = 0; i < 9; i++) {
				glTexCoord2fv(flatFaceTextureVertices[i % 8].data());
				glVertex3fv(vertices[zPolarityIndex][i % 8].data());
			}
			glEnd();
		}
	}

	// Upright Faces
	{
		const static std::array<TextureVertex, 4> TEXTURE_VERTICES{
			{
				TextureVertex{0, 0},
				TextureVertex{1, 0},
				TextureVertex{1, 1},
				TextureVertex{0, 1},
			}
		};

		// connect vertices opposing each other across xz-plane
		for (size_t faceIndex = 0; faceIndex < 8; faceIndex++) {
			glBegin(GL_POLYGON);
			glNormal3dv((GLdouble*)uprightFacesNormalVectors[faceIndex].data());
			for (size_t zPolarityIndex = 0; zPolarityIndex < 2; zPolarityIndex++) {
				for (size_t i = 0; i < 2; i++) {
					glTexCoord2fv(TEXTURE_VERTICES[zPolarityIndex * 2 + i].data());
					glVertex3fv(vertices[zPolarityIndex][(faceIndex + i + zPolarityIndex - (zPolarityIndex && i) * 2) % 8].data());
				}
			}
			glEnd();
		}
	}

	// Cage
	if (cageColor) {
		cageColor->render();

		glPushMatrix();
			glDepthFunc(GL_LEQUAL);
			glPushAttrib(GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
		
			glBegin(GL_LINES);
				for (size_t i = 0; i < 8; i++) {
					for (size_t zPolarityIndex = 0; zPolarityIndex < 2; zPolarityIndex++) {
						// draw edge along xz-plane
						glVertex3fv(vertices[zPolarityIndex][i].data());
						glVertex3fv(vertices[zPolarityIndex][(i + 1) % 8].data());
		
						// draw edge across xz-plane
						glVertex3fv(vertices[0][(i + zPolarityIndex) % 8].data());
						glVertex3fv(vertices[1][(i + zPolarityIndex) % 8].data());
					}
				}
			glEnd();
			glPopAttrib();
		glPopMatrix();
	}
}


////////////////////////////////////////////////////////////
/// Dev functions
////////////////////////////////////////////////////////////

void indicateCurrentPosition() {
	glPushMatrix();
	Color(0.f, 1.f, 0.f).render();
	//glScalef(0.1, 0.1, 0.1);
	Cube::draw();
	glPopMatrix();
}


void drawZVector() {
	glPushMatrix();
	glLineWidth(5);

	glBegin(GL_LINES);
	Color(0.f, 1.f, 0.f).render();
	glVertex3f(0, 0, 0);

	Color(1.f, 0.f, 0.f).render();
	glVertex3f(0, 3, 0);
	glEnd();
	glPopMatrix();
}
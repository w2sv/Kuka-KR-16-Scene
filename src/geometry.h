#pragma once

#include "utils.h"
#include "glutils.h"

#include "../dependencies/freeglut.h"

#include <array>


void drawCube();

void drawSquare();

void drawCylinder(float startRadius, float endRadius, float height);

class OctogonalPrism {
public:
	typedef std::array<GLfloat, 3> Vertex;
	typedef std::array<std::array<Vertex, 8>, 2> Vertices;

	typedef std::array<GLfloat, 2> TextureVertex;
	typedef std::array<TextureVertex, 8> FlatFaceTextureVertices;

	OctogonalPrism(float heigth, float straightEdgeLength, float diagonalEdgeLength);
	void draw(const Color& corpusColor, const Color* cageColor = nullptr) const;
private:
	Vertices vertices;
	Vertices calculateVertices(float lateralLength, float intermediateCoordinate, float z);
	
	FlatFaceTextureVertices flatFaceTextureVertices;
	FlatFaceTextureVertices calculateFlatFaceTextureVertices(float lateralLength, float intermediateCoordinate);

	static const std::array<TextureVertex, 4> UPRIGHT_FACES_TEXTURE_VERTICES;
};

void drawQuadraticGrid(const Extrema& limits, int tiles, const Color& color);

void drawPlane(const Extrema& xExtrema, const Extrema& yExtrema);

////////////////////////////////////////////////////////////
/// Dev functions
////////////////////////////////////////////////////////////

#ifdef DEBUG
void indicateCurrentPosition();

void drawZVector();
#endif
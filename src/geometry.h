#pragma once

#include "utils.h"
#include "glutils.h"
#include "image.h"

#include <array>


namespace Cube {
	enum Face {
		Front = 0,
		Back = 1,
		Left = 2,
		Right = 3,
		Top = 4,
		Bottom = 5,
	};

	void draw();
	void drawFace(Face face);
}

void drawSquare();

void drawCylinder(float startRadius, float endRadius, float height);

class OctogonalPrism {
public:
	OctogonalPrism(float heigth, float straightEdgeLength, float diagonalEdgeLength);
	void draw(const Color& corpusColor, const Color* cageColor = nullptr) const;
private:
	typedef std::array<GLfloat, 3> Vertex;
	typedef std::array<std::array<Vertex, 8>, 2> Vertices;

	Vertices vertices;
	Vertices calculateVertices(float lateralLength, float intermediateCoordinate, float z);
	
	typedef std::array<Vector3, 8> UprightFaceNormalVectors;
	UprightFaceNormalVectors uprightFacesNormalVectors;
	UprightFaceNormalVectors calculateUprightFacesNormalVectors() const;

	typedef std::array<GLfloat, 2> TextureVertex;
	typedef std::array<TextureVertex, 8> FlatFaceTextureVertices;
	FlatFaceTextureVertices flatFaceTextureVertices;
	FlatFaceTextureVertices calculateFlatFaceTextureVertices(float lateralLength, float intermediateCoordinate);
};

void drawQuadraticGrid(const Extrema& limits, int tiles, const Color& color);

void drawPlane(const Extrema& xExtrema, const Extrema& yExtrema);

////////////////////////////////////////////////////////////
/// Dev functions
////////////////////////////////////////////////////////////

void indicateCurrentPosition();

void drawZVector();
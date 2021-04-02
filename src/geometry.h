#pragma once

#include "utils.h"
#include "glutils.h"
#include "image.h"

#include <array>


namespace Cube {
	enum Face {
		Right = 0,
		Left = 1,
		Top = 2,
		Bottom = 3,
		Back = 4,
		Front = 5,
	};

	void draw(bool texture3D = false);
	void drawFace(Face face, bool texture3D);
}

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

void drawQuadraticPlane(const Extrema& extrema, int nSubSquares);

////////////////////////////////////////////////////////////
/// Dev functions
////////////////////////////////////////////////////////////

void indicateCurrentPosition();

void drawZVector();
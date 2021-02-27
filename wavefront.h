// Wavefront Loader
// PrakCG
#pragma once

#ifndef WAVEFRONT_H
#define WAVEFRONT_H

#include "glutils.h"


inline GLfloat* normieren ( GLfloat v[3] );
inline void crossProduct( GLfloat *n, GLfloat *a, GLfloat *b );

class cg_object3D
{
public:
	// constructor
	cg_object3D ();
	// destructor
	~cg_object3D ();

	// Laden eines Wavefront-Objektes aus einer Datei,
	// Benötigter Speicher wird in dieser Fkt. allokiert
	// rgb: Farbe
	// x,y,z: Position
	void load ( const char *filename, bool use_vbos );

	// Objekt Material + Farben setzen
	void setMaterial ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, GLfloat spec, GLfloat shine, GLfloat emis );
	void setMaterial(Color& color, GLfloat spec, GLfloat shine, GLfloat emis);

	// Objekt Position setzen
	void setPosition ( GLfloat x, GLfloat y, GLfloat z );

	// Objekt Normalenmodus setzen
	void setNormalmode ( int normalmode = 2 );

	// Freigeben des belegten Speichers
	void free ();

	// Zeichnen des Objektes
	void draw ();

private:
	unsigned short int  normal_mode;	// 0: keine Normalen, 1: Normalen per Surface, 2: Normalen per Vertex
	int   numpoints;					// Anzahl der Vertex-Zeilen im File
	int	  numnormals;					// Anzahl der Normalen-Zeilen im File
	int   numtris;						// Anzahl der Dreiecke im File

	bool  clustertexcoords;				// File enthält Clustervertices mit Texturkoordinatenindex
	bool  clusternormals;				// File enthält Clustervertices mit Normalenindex

										// Daten:
	GLfloat *points;					// Liste der importierten Vertex-Koordinaten
	GLfloat *normals;					// Liste der importierten Normalen (Wavefront)
	GLfloat* texcoords;					// Liste der importierten Texturkoordinaten (Wavefront)

	GLfloat* f_normals;					// Liste der berechneten Flächennormalen
	GLfloat* p_normals;					// Liste der berechneten Vertexnormalen (nur wenn clusternormals == false)

	int   *tris;						// Liste der Dreieck (Index)
	int	  *i_normals;					// Liste der importierten Vertexnormalen für die Dreieckspunkte (Index)
	int   *i_texcoords;					// Liste der importierten Texturkoordinaten für die Dreieckspunkte (Index)

	bool vbo_geladen;
	GLuint vbo;							// VBO [Vertices - Vertexnormalen - Surfacenormalen - Texturkoordinaten]

	GLfloat color[4];					// Materialdefinition
	GLfloat amb[4];			
	GLfloat diff[4];
	GLfloat spec[4];
	GLfloat shine;
	GLfloat emis[4];

	GLfloat pos[3];						// Position

	bool readClusterVertex( char **r_line, int *p, int *t, int *n );
};

#endif // WAVEFRONT_H

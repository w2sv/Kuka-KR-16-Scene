#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <iostream>

#include "../dependencies/glew.h"
#include "../dependencies/freeglut.h"

#define EXPORT_EXT
#include "wavefront.h"
#include "utils.h"

/* wavefront */

inline GLfloat* normieren(GLfloat v[3])
{
	GLfloat l = 1.0f / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= l;
	v[1] *= l;
	v[2] *= l;
	return v;
}

inline void crossProduct(GLfloat* n, GLfloat* a, GLfloat* b)
{
	n[0] = a[1] * b[2] - a[2] * b[1];
	n[1] = a[2] * b[0] - a[0] * b[2];
	n[2] = a[0] * b[1] - a[1] * b[0];
}

cg_object3D::cg_object3D()
{
	points = nullptr;
	normals = nullptr;
	texcoords = nullptr;

	f_normals = nullptr;
	p_normals = nullptr;

	tris = nullptr;

	setMaterial(0.6f, 0.6f, 0.6f, 1.0f, 0.0f, 128.0f, 0.0f);
	setPosition(0.0f, 0.0f, 0.0f);
	setNormalmode(2);
	numtris = 0;
}


cg_object3D::~cg_object3D()
{
	free();
}

void cg_object3D::setMaterial(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, GLfloat specular, GLfloat shininess, GLfloat emission)
{
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;
	amb[0] = 0.2 * red;
	amb[1] = 0.2 * red;
	amb[2] = 0.2 * red;
	amb[3] = alpha;
	diff[0] = red;
	diff[1] = green;
	diff[2] = blue;
	diff[3] = alpha;
	spec[0] = specular;
	spec[1] = specular;
	spec[2] = specular;
	spec[3] = alpha;
	shine = shininess;
	// Emission = r,g,b * emis
	emis[0] = red * emission;
	emis[1] = green * emission;
	emis[2] = blue * emission;
	emis[3] = alpha;
}


void cg_object3D::setMaterial(Color& color, GLfloat spec, GLfloat shine, GLfloat emis) {
	setMaterial(color.r, color.g, color.b, 1., spec, shine, emis);
}


void cg_object3D::setPosition(GLfloat x, GLfloat y, GLfloat z)
{
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
}


void cg_object3D::setNormalmode(int normalmode)
{
	normal_mode = normalmode;
}


bool cg_object3D::readClusterVertex(char** r_line, int* p, int* t, int* n)
{

	char* h = r_line[0];										// Zeiger auf den Anfang der Zeile
	p[0] = -1;
	t[0] = -1;
	n[0] = -1;

	while (h[0] != 0 && h[0] == ' ') h++;							// überlese führende Blanks

	if (sscanf(h, "%i", &p[0]) == 1)  							// Vertex-Index wurde gelesen, es folgt ein Slash oder ein Blank bzw. das Zeilenende
	{
		// Wenn ein Slash folgt, dann haben wir einen Vertex-Cluster

		while (h[0] != 0 && h[0] != '/' && h[0] != ' ') h++;		// Vorsetzen bis Zeilenende, Slash oder Leerzeichen

		switch (h[0])
		{
		case 0:												// Zeilenende erreicht
			r_line[0] = h;
			return true;
		case ' ':											// Leerzeichen, d.h. kein Vertex-Cluster
			r_line[0] = h;
			return true;
		case '/':											// Vertex-Cluster
			h++;											// den Slash überlesen, es folgt  die Texturkoordinate oder ein zweiter Slash

			sscanf(h, "%i", &t[0]);							// wir versuchen die Texturkoordinate zu lesen
			while (h[0] != 0 && h[0] != '/' && h[0] != ' ') h++;	// zum zweiten Slash vorsetzen

			if (h[0] == '/')  								// die Vertexnormale muss eigentlich folgen, wenn sie fehlt setzen wir die Standardnormale
			{
				h++;										// den zweiten Slash überlesen
				if (h[0] == ' ')  							// Fehler: Normale fehlt
				{
					n[0] = 0;
				}
				else
				{
					sscanf(h, "%i", &n[0]);
					while (h[0] != 0 && h[0] != ' ') h++;		// über die Vertexnormale vorsetzen
				}
				r_line[0] = h;
				return true;
			}
			break;
		default:
			p[0] = -1;
			r_line[0] = h;
			return false;
		}
	}
	r_line[0] = h;
	return false;
}


void cg_object3D::load(const char* filename, bool use_vbos)
{
	FILE* f = fopen(filename, "r");
	if (!f) {
		std::cerr << filename << "does not exist" << std::endl;
		std::getchar();
		std::exit(1);
	};

	int npoints = 0, ntris = 0, nnormals = 0, ntexcoords = 0;

#define ZEILENLAENGE 200
	char s[ZEILENLAENGE];

	bool clnormals = false;
	bool cltexcoords = false;

	// Pass1: Speicherbedarf ermitteln
	int line = 0;
	while (!feof(f))
	{
		fgets(s, ZEILENLAENGE - 1, f);					// eine Zeile lesen
		++line;
		switch (s[0])
		{
		case 'v':
			switch (s[1])
			{
			case ' ':
				npoints++;
				break;
			case 'n':
				clnormals = true;
				nnormals++;
				break;
			case 't':
				cltexcoords = true;
				ntexcoords++;
				break;
			}
			break;
		case 'f':
			int p[3];
			int n[3];
			int t[3];

			char* h = &s[1];	// starte am Zeichen s[1]

			if (readClusterVertex(&h, &p[0], &t[0], &n[0]) &&
				readClusterVertex(&h, &p[1], &t[1], &n[1]) &&
				readClusterVertex(&h, &p[2], &t[2], &n[2]))
			{
				++ntris;										// erstes Dreieck
				// TODO: Polygone zerlegen
				while (readClusterVertex(&h, &p[0], &t[0], &n[0]))	// weitere Punkte
					ntris++;
			}
			else
			{
				printf("Fehler in Wavefrontdatei %s in Zeile %d\n", filename, line);
				return;
			}
			break;
		}
	}

	// #define __DEBUG
#ifdef __DEBUG
	printf("OBJ Loader PASS 1\n");
	printf("npoints: %d, ntris: %d, nnormals: %d, ntexcoords: %d\n", npoints, ntris, nnormals, ntexcoords);
#endif

	// PASS2: Punkte, Normalen und Texturkoordinaten laden
	if (npoints > 0 && ntris > 0)
	{
		points = (GLfloat*)malloc(npoints * 3 * sizeof(GLfloat));							// Punktarray anlegen
		tris = (GLint*)malloc(ntris * 3 * sizeof(GLint));									// Index-Array für Flächen anlegen

		if (cltexcoords)
		{
			//use_vbos = false;
			clustertexcoords = true;
			i_texcoords = (GLint*)malloc(3 * ntris * sizeof(GLint));					// importierte TexturkoordinatenpIndexe
			texcoords = (GLfloat*)malloc((ntexcoords + 1) * 2 * sizeof(GLfloat));		// u,v - Koordinaten aus dem File
			memset(i_texcoords, 0, 3 * ntris * sizeof(GLint));
			memset(texcoords, 0, (ntexcoords + 1) * 2 * sizeof(GLfloat));
		}

		if (clnormals)
		{
			//use_vbos = false;
			clusternormals = true;
			nnormals++;																// wir nehmen noch die Standardnormale dazu
			i_normals = (GLint*)malloc(3 * ntris * sizeof(GLint));					// importierte NormalenpIndexe
			normals = (GLfloat*)malloc((nnormals) * 3 * sizeof(GLfloat));			// nx, ny, nz aus dem File
			memset(i_normals, 0, 3 * ntris * sizeof(GLint));
			memset(normals, 0, (nnormals) * 3 * sizeof(GLfloat));
			normals[0] = 0;
			normals[1] = 0;
			normals[2] = 1;						// Standard-Normale
		}

		int hnormals = 1;												// die Standard-Normale steht im Index 0
		npoints = 0;
		ntris = 0;
		ntexcoords = 0;													// Initialisierung

		rewind(f);

		line = 0;
		while (!feof(f))
		{
			fgets(s, ZEILENLAENGE - 1, f);					// eine Zeile lesen
			++line;

			switch (s[0])
			{
			case 'v':
				switch (s[1])
				{
				case ' ':
				{
					GLfloat* p = &points[3 * npoints];									// pointer auf Vertex setzen
					if (sscanf(&s[2], "%f %f %f", &p[0], &p[1], &p[2]) == 3)					// 3 Floats lesen
						npoints++;
					else
					{
						printf("Fehler in Wavefrontdatei %s in Zeile %d\n", filename, line);
						return;
					}
				}
				break;
				case 'n':
				{
					GLfloat* n = &normals[3 * (hnormals)];
					if (sscanf(&s[2], "%f %f %f", &n[0], &n[1], &n[2]) == 3)					// 3 Floats lesen
						hnormals++;
					else
					{
						printf("Fehler in Wavefrontdatei %s in Zeile %d\n", filename, line);
						return;
					}
				}
				break;
				case 't':
				{
					GLfloat* t = &texcoords[2 * (ntexcoords + 1)];
					if (sscanf(&s[2], "%f %f", &t[0], &t[1]) == 2)							// 2 Floats lesen
						ntexcoords++;
					else
					{
						printf("Fehler in Wavefrontdatei %s in Zeile %d\n", filename, line);
						return;
					}
				}
				break;
				}
				break;
			case 'f':
				int* p = &tris[3 * ntris];
				int* on = &i_normals[3 * ntris];
				int* ot = &i_texcoords[3 * ntris];
				int n[3];
				int t[3];

				char* h = &s[1];	// starte am Zeichen s[1]

				if (readClusterVertex(&h, &p[0], &t[0], &n[0]) &&
					readClusterVertex(&h, &p[1], &t[1], &n[1]) &&
					readClusterVertex(&h, &p[2], &t[2], &n[2]))
				{
					--p[0];
					--p[1];
					--p[2];		// Indizes dekrementieren (1->0)
					for (int i = 0; i < 3; i++)  // Textur- und Normalenindizes nur dekrementieren, wenn gelesen
					{
						if (cltexcoords)
						{
							if (t[i] > 0)
							{
								if (t[i] <= ntexcoords) // Index liegt im gültigen Bereich
									ot[i] = t[i];		// sonst steht da (-1)
								else
								{
									//printf("Index zu groß, korrigiert\n\r");
									ot[i] = 0;
								}
							}
							else
								ot[i] = 0;	// die Standard-Texturkoordinate 0,0 steht im Index 0
						}

						if (clnormals)
						{
							if (n[i] > 0)
								on[i] = n[i];
							else
								on[i] = 0;											// die Standardnormale 0,0,1 steht im Index 0
						}
					}
					++ntris;														// erstes Dreieck gelesen
					// TODO: Surfaces mit mehr als 3 Ecken zerlegen
					int hp;
					int ht;
					int hn;
					while (readClusterVertex(&h, &hp, &ht, &hn))  				// weitere Punkte
					{
						int* q = &tris[3 * ntris];									// das nächste Dreieck
						q[0] = p[0];
						q[1] = p[2];
						q[2] = hp - 1;
						p = q;
						if (clnormals)
						{
							int* hon = &i_normals[3 * ntris];
							hon[0] = on[0];
							hon[1] = on[2];
							hon[2] = hn;
							on = hon;
						}
						if (cltexcoords)
						{
							int* hot = &i_texcoords[3 * ntris];
							hot[0] = ot[0];
							hot[1] = ot[2];
							hot[2] = ht;
							ot = hot;
						}
						ntris++;
					}
				}
				else
				{
					printf("Fehler in Wavefrontdatei %s in Zeile %d\n", filename, line);
					return;
				}
				break;
			}
		}

		numpoints = npoints;
		numtris = ntris;
		numnormals = nnormals;

		//printf("npoints: %d, ntris: %d\n", npoints, ntris);

	}
	// END PASS 2

	fclose(f);

	//#define __DEBUG
#ifdef __DEBUG

	printf("\nOBJ Loader PASS 2\n");

	printf("Vertices: \n");
	for (int i = 0; i < numpoints; i++)
	{
		printf("Index: %d, x: %3.5f, y: %3.5f, z: %3.5f\n", i, points[3 * i], points[3 * i + 1], points[3 * i + 2]);
	};

	printf("\n\nNormals: \n");
	for (int i = 0; i < numnormals; i++)
	{
		printf("Index: %d, x: %3.5f, y: %3.5f, z: %3.5f\n", i, normals[3 * i], normals[3 * i + 1], normals[3 * i + 2]);
	};

	printf("\n\nTexCoords: \n");
	for (int i = 0; i < ntexcoords; i++)
	{
		printf("Index: %d, u: %3.3f, v: %3.3f\n", i, texcoords[2 * i], texcoords[2 * i + 1]);
	};

	printf("\n\nTRIs: \n");
	for (int i = 0; i < numtris; i++)
	{
		if (clustertexcoords)
			printf("Index: %d, a: %d/%d/%d, b: %d/%d/%d, c: %d/%d/%d\n", i,
				tris[3 * i], i_texcoords[3 * i], i_normals[3 * i],
				tris[3 * i + 1], i_texcoords[3 * i + 1], i_normals[3 * i + 1],
				tris[3 * i + 2], i_texcoords[3 * i + 2], i_normals[3 * i + 2]
			);
	};
#endif

#pragma region NORMALEN_BERECHNEN
	// wir berechnen die Surface-Normalen immer

	// calculate surface normals (one normal per surface)
	// let's start with surface normals

	f_normals = (GLfloat*)malloc(3 * ntris * sizeof(GLfloat));
	for (int i = 0; i < ntris; i++)
	{
		// calculate edges per triangle

		int* act_tri = &(tris[3 * i]);
		GLfloat* p0 = &(points[3 * act_tri[0]]);
		GLfloat* p1 = &(points[3 * act_tri[1]]);
		GLfloat* p2 = &(points[3 * act_tri[2]]);

		GLfloat v1[3];
		v1[0] = p1[0] - p0[0];
		v1[1] = p1[1] - p0[1];
		v1[2] = p1[2] - p0[2];

		GLfloat v2[3];
		v2[0] = p2[0] - p0[0];
		v2[1] = p2[1] - p0[1];
		v2[2] = p2[2] - p0[2];

		GLfloat* act_n = &(f_normals[3 * i]);
		crossProduct(act_n, v1, v2);
		normieren(act_n);
	}

	if (!clusternormals)
	{	// wir müssen interpolierte Normalen berechnen
		// now let's continue with interpolated normals per vertex
		p_normals = (GLfloat*)malloc(3 * npoints * sizeof(GLfloat));
		memset(p_normals, 0, 3 * npoints * sizeof(GLfloat));
		for (int i = 0; i < npoints; i++)
		{
			// for each point accumulate the face normals of faces the act. point is member of
			GLfloat* act_n = &(p_normals[3 * i]);
			for (int j = 0; j < ntris; j++)
			{
				//loop over tris and check if p is member of act triangle
				int* act_tri = &(tris[3 * j]);
				if ((act_tri[0] == i) || (act_tri[1] == i) || (act_tri[2] == i))
				{
					// add face normal
					GLfloat* act_fn = &(f_normals[3 * j]);
					act_n[0] += act_fn[0];
					act_n[1] += act_fn[1];
					act_n[2] += act_fn[2];
				}
			}
			normieren(act_n);
		}
	}
#pragma endregion NORMALEN_BERECHNEN

#pragma region VBO_Erzeugen
	//////////////////////////////////////////////////
	// Objekt geladen: Erzeugen und Laden nun die VBOs
	//////////////////////////////////////////////////
	if (use_vbos)
	{
		// wir müssen die Daten für Drawcalls aufbereiten, weil ein Vertex mit mehreren Normalen und TCs vorkommen kann
		// (beim vertex split)

		// wir erzeugen ein VBO mit der Struktur [points, normals, {texcoords}]

		int vertexArraySize = numtris * 9;	// 3 Punkte pro Dreieck mit x,y,z
		int normalArraySize = vertexArraySize; 	// dto.
		int texcoordArraySize = 0;
		if (clustertexcoords)
			texcoordArraySize = numtris * 6; // 3 Punkte mit u,v
		else texcoordArraySize = 0;

		int vboDataSize = vertexArraySize + 2 * normalArraySize + texcoordArraySize;
		GLfloat* vbodata = (GLfloat*)malloc(vboDataSize * sizeof(GLfloat));
		memset(vbodata, 0, vboDataSize * sizeof(GLfloat));

		int vIndex = 0;
		int npIndex = vertexArraySize;
		int nfIndex = 2 * vertexArraySize;
		int tIndex = 0;
		if (clustertexcoords)
			tIndex = 2 * vertexArraySize + normalArraySize;

		int* p = NULL;
		int* in = NULL;
		int* it = NULL;
		// über alle Punkte der Dreiecke, Normalen und ggf. Texturkoordinaten ins VBO kopieren
		for (int i = 0; i < numtris; ++i)
		{
			p = &tris[3 * i];
			if (clusternormals)
				in = &i_normals[3 * i];
			else
				in = &tris[3 * i];
			if (clustertexcoords)
				it = &i_texcoords[3 * i];

			GLfloat* fn = &f_normals[3 * i];			// Zeiger auf die Flächennormale
			for (int j = 0; j < 3; j++) {
				GLfloat* v = &points[3 * p[j]];
				GLfloat* n = &normals[3 * in[j]];		// der Zeiger auf die Vertexnormale
				GLfloat* tc = NULL;
				if (clustertexcoords)
					tc = &texcoords[2 * it[j]];	// der Zeiger auf die TC

				memcpy(&vbodata[vIndex], v, 3 * sizeof(GLfloat));
				memcpy(&vbodata[npIndex], n, 3 * sizeof(GLfloat));
				memcpy(&vbodata[nfIndex], fn, 3 * sizeof(GLfloat));
				if (clustertexcoords)
					memcpy(&vbodata[tIndex], &tc[0], 2 * sizeof(GLfloat));

				vIndex += 3;
				npIndex += 3;
				nfIndex += 3;
				if (clustertexcoords) tIndex += 2;
			}
		}

		// Step 1: VBOs erzeugen
		glGenBuffers(1, &vbo);

		if (vbo) // kein Fehler beim Generieren
		{
			// Vertex Buffer binden
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			// Vertexdaten übertragen (Koordinaten, dann Vertexnormalen, dann Texturkoordinaten)
			glBufferData(GL_ARRAY_BUFFER, vboDataSize * sizeof(GLfloat), vbodata, GL_STATIC_DRAW);

			::free(vbodata);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			vbo_geladen = 1;
		}
#pragma endregion VBO_Erzeugen
	}
	if (clusternormals)
		normal_mode = 2;
}

void cg_object3D::free()
{
	if (points) ::free(points);
	if (tris) ::free(tris);
	if (f_normals) ::free(f_normals);
	if (p_normals) ::free(p_normals);
	if (texcoords) ::free(texcoords);
}

// Zeichnet ein Wavefront-Object *obj
void cg_object3D::draw()
{
	if (points)
	{
		int* p = NULL;
		float* n = NULL;

		glPushMatrix();

		glTranslatef(pos[0], pos[1], pos[2]);

		glColor4fv(diff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

		if (!vbo_geladen)  // haben keine VBOs
		{
			glBegin(GL_TRIANGLES);
			for (int i = 0; i < numtris; ++i)
			{
				switch (normal_mode)
				{
				case 0:
					//set no normals
					p = &tris[3 * i];
					glVertex3fv(&points[3 * p[0]]);
					glVertex3fv(&points[3 * p[1]]);
					glVertex3fv(&points[3 * p[2]]);
					break;
				case 1:	//set surface normal
					n = &(f_normals[3 * i]);
					glNormal3fv(n);
					p = &tris[3 * i];
					glVertex3fv(&points[3 * p[0]]);
					glVertex3fv(&points[3 * p[1]]);
					glVertex3fv(&points[3 * p[2]]);
					break;
				case 2:
					if ((clusternormals) && (clustertexcoords))  		// Normalen und Texturkoordinaten
					{
						p = &tris[3 * i];
						int* in = &i_normals[3 * i];
						int* it = &i_texcoords[3 * i];

						glNormal3fv(&normals[3 * in[0]]);
						glTexCoord2fv(&texcoords[2 * it[0]]);
						glVertex3fv(&points[3 * p[0]]);

						glNormal3fv(&normals[3 * in[1]]);
						glTexCoord2fv(&texcoords[2 * it[1]]);
						glVertex3fv(&points[3 * p[1]]);

						glNormal3fv(&normals[3 * in[2]]);
						glTexCoord2fv(&texcoords[2 * it[2]]);
						glVertex3fv(&points[3 * p[2]]);
					}
					else if (clusternormals)  								// Normalen ohne Texturen
					{
						p = &tris[3 * i];
						int* in = &i_normals[3 * i];
						glNormal3fv(&normals[3 * in[0]]);
						glVertex3fv(&points[3 * p[0]]);
						glNormal3fv(&normals[3 * in[1]]);
						glVertex3fv(&points[3 * p[1]]);
						glNormal3fv(&normals[3 * in[2]]);
						glVertex3fv(&points[3 * p[2]]);
					}
					else  														// ohne Normalen und Texturkoordinaten
					{
						p = &tris[3 * i];
						// set vertex normals
						glNormal3fv(&p_normals[3 * p[0]]);
						glVertex3fv(&points[3 * p[0]]);
						glNormal3fv(&p_normals[3 * p[1]]);
						glVertex3fv(&points[3 * p[1]]);
						glNormal3fv(&p_normals[3 * p[2]]);
						glVertex3fv(&points[3 * p[2]]);
					}

					break;
				}
			}
			glEnd();
		}
		else  															// wir haben VBOs geladen
		{
			// VBO binden
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			// Client State aktivieren
			if (clustertexcoords)
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			if (normal_mode == 1 || normal_mode == 2)
				glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);

			// Pointer spezifizieren
			int vCount = numtris * 3;  // 3 Punkte pro Dreieck
			if (clustertexcoords)
				glTexCoordPointer(2, GL_FLOAT, 0, (GLfloat*)(9 * vCount * sizeof(GLfloat)));
			if (normal_mode == 1)
				glNormalPointer(GL_FLOAT, 0, (GLfloat*)(6 * vCount * sizeof(GLfloat)));
			if (normal_mode == 2)
				glNormalPointer(GL_FLOAT, 0, (GLfloat*)(3 * vCount * sizeof(GLfloat)));
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			// Zeichnen
			glDrawArrays(GL_TRIANGLES, 0, vCount);

			// Client State deaktivieren
			if (clustertexcoords)
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			if (normal_mode == 1 || normal_mode == 2)
				glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		glPopMatrix();
	}
}

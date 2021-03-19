#include "light.h"


// Farbe setzen fuer Farb- und Beleuchtungsmodus
void setColor(GLfloat r, GLfloat g, GLfloat b) {
	glColor3f(r, g, b);
	GLfloat amb_diff[4] = { r, g, b, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, amb_diff);
}


void setMaterial(GLenum face, GLfloat amb[4], GLfloat diff[4], GLfloat spec[4], GLfloat shine, GLfloat emis[4])
// Aktualisierung des OpenGL Materials
{
	glMaterialfv(face, GL_AMBIENT, amb);
	glMaterialfv(face, GL_DIFFUSE, diff);
	glMaterialfv(face, GL_SPECULAR, spec);
	glMaterialf(face, GL_SHININESS, shine);
	glMaterialfv(face, GL_EMISSION, emis);
}

void setLights()
{
	// Parameter eines globalen Lichts
	GLint g_localviewer = GL_TRUE;
	GLfloat g_amb[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, g_localviewer);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_amb);

	// Licht 0

	// Paramaters fuer Lichtquelle 0
	GLfloat l_pos[4] = { -1.0f, 1.0f, 1.0f, 0.0f };
	GLfloat l_amb[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat l_diff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l_spec[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l_spotdir[3] = { 0.0f, 0.0f, 1.0f };
	GLfloat l_spotcutoff = 360.0f;
	GLfloat l_spotexp = 0.0f;
	GLfloat l_att[3] = { 1.0f, 0.0f, 0.0f };

	// Position
	glLightfv(GL_LIGHT0, GL_POSITION, l_pos);
	// Farbe
	glLightfv(GL_LIGHT0, GL_AMBIENT, l_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l_diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l_spec);
	// Spot
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l_spotdir);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, l_spotcutoff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, l_spotexp);
	// Abschwaechung
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, l_att[0]);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, l_att[1]);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, l_att[2]);
	// Aktivieren
	glEnable(GL_LIGHT0);
}

// Standardeinstellung für Farbe und Beleuchtung
void setDefaultLightAndMaterial(GLboolean lightMode) {
	GLfloat color[4] = { 1, 1, 1, 1 };

	if (lightMode == GL_TRUE){
		GLfloat m_amb[4] = { 0.2, 0.2, 0.2, 1.0 };
		GLfloat* m_diff = color;
		GLfloat m_spec[4] = { 0.2, 0.2, 0.2, 1.0 };
		GLfloat m_shine = 32.0;
		GLfloat m_emiss[4] = { 0.0, 0.0, 0.0, 1.0 };

		setMaterial(GL_FRONT_AND_BACK, m_amb, m_diff, m_spec, m_shine, m_emiss);

		setLights();

		glEnable(GL_NORMALIZE);
		glEnable(GL_LIGHTING);
	}
	else {
		glDisable(GL_LIGHTING);
		glColor4fv(color);
	}
};
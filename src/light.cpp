#include "light.h"

#include "glutils.h"


Material::Material(RGBAParameter amb, RGBAParameter diff, RGBAParameter spec, GLfloat shine, RGBAParameter emis) :
	amb(amb),
	diff(diff),
	spec(spec),
	shine(shine),
	emis(emis)
{}


void Material::set() const {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb.data());
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff.data());
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec.data());
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis.data());
}


void setDefaultLightAndMaterial(GLboolean lightMode) {
	RGBAParameter color = { 1, 1, 1, 1. };
	
	if (lightMode == GL_TRUE){
		static const Material DEFAULT_MATERIAL(
			RGBAParameter{ 0.2, 0.2, 0.2, 1.0 },
			RGBAParameter{ 0.2, 0.2, 0.2, 1.0 },
			color,
			32.f,
			RGBAParameter{ 0.0, 0.0, 0.0, 1.0 }
		);

		DEFAULT_MATERIAL.set();

		setLight();

		glEnable(GL_NORMALIZE);
		glEnable(GL_LIGHTING);
	}
	else {
		glDisable(GL_LIGHTING);
		glColor4fv(color.data());
	}
};


void setLight() {
	static const RGBAParameter color = { 0.3, 0.3, 0.3, 1. };

	// set light model parameters
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color.data());

	static cg_light light(0);
	light.setPosition(1.0f, 1.0f, 1.0f, 0.0f);

	light.setAmbient(0.7f, 0.7f, 0.7f, 1.0f);
	light.setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	light.setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

	light.setAttentuation(1.0f, 5, 10);

	light.draw();
}


// Abstraktion der OpenGL Lichtquellen
cg_light::cg_light(int num)
{
	this->id = GL_LIGHT0 + num;
	setPosition(0.0f, 0.0f, 1.0f, 0.0f);
	setAmbient(0.0f, 0.0f, 0.0f, 1.0f);
	setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
	setSpotlight(0.0f, -1.0f, 0.0f, 180.0f, 0.0f);
	setAttentuation(1.0f, 0.0f, 0.0f);
}


void cg_light::setPosition(float x, float y, float z, float w)
{
	this->pos[0] = x;
	this->pos[1] = y;
	this->pos[2] = z;
	this->pos[3] = w;
}


void cg_light::setAmbient(float r, float g, float b, float a)
{
	this->amb[0] = r;
	this->amb[1] = g;
	this->amb[2] = b;
	this->amb[3] = a;
}


void cg_light::setDiffuse(float r, float g, float b, float a)
{
	this->diff[0] = r;
	this->diff[1] = g;
	this->diff[2] = b;
	this->diff[3] = a;
}


void cg_light::setSpecular(float r, float g, float b, float a)
{
	this->spec[0] = r;
	this->spec[1] = g;
	this->spec[2] = b;
	this->spec[3] = a;
}


void cg_light::setSpotlight(float directionX, float directionY, float directionZ, float cutoff, float exponent)
{
	this->spot[0] = directionX;
	this->spot[1] = directionY;
	this->spot[2] = directionZ;
	this->spot[3] = cutoff;
	this->spot[4] = exponent;
}


void cg_light::setAttentuation(float constant, float linear, float quadric)
{
	this->att[0] = constant;
	this->att[1] = linear;
	this->att[2] = quadric;
}

void cg_light::draw()
{
	glPushMatrix();
		// position
		glLightfv(this->id, GL_POSITION, this->pos);
		// color
		glLightfv(this->id, GL_AMBIENT, this->amb);
		glLightfv(this->id, GL_DIFFUSE, this->diff);
		glLightfv(this->id, GL_SPECULAR, this->spec);
		// spotlight
		glLightfv(this->id, GL_SPOT_DIRECTION, this->spot);
		glLightf(this->id, GL_SPOT_CUTOFF, this->spot[3]);
		glLightf(this->id, GL_SPOT_EXPONENT, this->spot[4]);
		// attentuation
		glLightf(this->id, GL_CONSTANT_ATTENUATION, this->att[0]);
		glLightf(this->id, GL_LINEAR_ATTENUATION, this->att[1]);
		glLightf(this->id, GL_QUADRATIC_ATTENUATION, this->att[2]);
		// enable
		glEnable(this->id);
	glPopMatrix();
}


void cg_light::markLightPosition() {
	// eine kleine Kugel an die Position der Lichtquelle zeichnen
	glPushMatrix();
	glTranslatef(this->pos[0], this->pos[1], this->pos[2]),
		glScalef(0.2, 0.2, 0.2);
	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glColor4fv(this->diff);
	glutSolidSphere(0.5, 30, 30);
	glPopAttrib();
	glPopMatrix();
}
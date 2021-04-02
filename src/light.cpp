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
			color,
			RGBAParameter{ 0.2, 0.2, 0.2, 1.0 },
			RGBAParameter{ 0.2, 0.2, 0.2, 1.0 },
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

	static const cg_light light(
		0,
		RGBAParameter{ 15.0f, 5.0f, 1.0f, 0.0f },
		RGBAParameter{ 0.7f, 0.7f, 0.7f, 1.0f },
		RGBAParameter{ 1.0f, 1.0f, 1.0f, 1.0f },
		RGBAParameter{ 1.0f, 1.0f, 1.0f, 1.0f },
		XYZParameter{ 1.0f, 5, 10 }
	);

	light.draw();
}


// Abstraktion der OpenGL Lichtquellen
cg_light::cg_light(
	int num, 
	RGBAParameter position, 
	RGBAParameter amb, 
	RGBAParameter diff, 
	RGBAParameter spec,
	XYZParameter attenuation,
	XYZParameter spotDirection,
	float spotCutoff,
	float spotExp
):
	id(GL_LIGHT0 + num),

	pos(position),

	amb(amb),
	diff(diff),
	spec(spec),

	att(attenuation),

	spotDir(spotDirection),
	spotCutoff(spotCutoff),
	spotExp(spotExp)
{
}


void cg_light::draw() const
{
	glPushMatrix();
		// enable
		glEnable(this->id);

		// position
		glLightfv(this->id, GL_POSITION, this->pos.data());

		// color
		glLightfv(this->id, GL_AMBIENT, this->amb.data());
		glLightfv(this->id, GL_DIFFUSE, this->diff.data());
		glLightfv(this->id, GL_SPECULAR, this->spec.data());

		// spotlight
		glLightfv(this->id, GL_SPOT_DIRECTION, this->spotDir.data());
		glLightf(this->id, GL_SPOT_CUTOFF, this->spotCutoff);
		glLightf(this->id, GL_SPOT_EXPONENT, this->spotExp);

		// attentuation
		glLightf(this->id, GL_CONSTANT_ATTENUATION, this->att.data()[0]);
		glLightf(this->id, GL_LINEAR_ATTENUATION, this->att.data()[1]);
		glLightf(this->id, GL_QUADRATIC_ATTENUATION, this->att.data()[2]);

	glPopMatrix();
}


void cg_light::disable() const {
	glDisable(id);
}


void cg_light::markLightPosition() {
	// eine kleine Kugel an die Position der Lichtquelle zeichnen
	glPushMatrix();
	glTranslatef(this->pos[0], this->pos[1], this->pos[2]),
		glScalef(0.2, 0.2, 0.2);
	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glColor4f(1, 0, 0, 1);
	glutSolidSphere(20, 30, 30);
	glPopAttrib();
	glPopMatrix();
}
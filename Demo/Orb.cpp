#include "Orb.h"

Orb::Orb(Vector pos, Vector vel, float damp, float iMass, bool ug, float rad, Vector c) : Particle(pos, vel, Vector(), damp, iMass, ug, Vector::GRAVITY), 
																						radius(rad), color(c)
{
}


Orb::~Orb()
{
}

gFloat Orb::CalcRadius() const
{
	return radius;
}

bool Orb::IsPointInside(Vector v)
{
	return (position - v).Magnitude() <= radius;
}

void Orb::Draw()
{
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glColor3f(color.x, color.y, color.z);
	glutSolidSphere(radius, 20, 20);
	glPopMatrix();
}
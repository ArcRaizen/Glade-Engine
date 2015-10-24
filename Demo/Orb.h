#pragma once
#ifndef ORB_H
#define ORB_H

#ifndef CORE_H
#include "Core.h"
#endif

using namespace Glade;

class Orb : public Particle
{
public:
	Orb(Vector pos, Vector vel, float damp, float iMass, bool ug, float rad, Vector color=Vector(0,0,1));
	virtual ~Orb();

	gFloat CalcRadius() const;
	bool IsPointInside(Vector v);
	void Draw();

private:
	float radius;
	Vector color;
};
#endif


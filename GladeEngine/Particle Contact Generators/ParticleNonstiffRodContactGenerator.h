#pragma once
#ifndef GLADE_PARTICLE_NONSTIFF_ROD_CONTACT_GENERATOR_H
#define GLADE_PARTICLE_NONSTIFF_ROD_CONTACT_GENERATOR_H

#ifndef GLADE_PARTICLE_ROD_CONTACT_GENERATOR_H
#include "ParticleRodContactGenerator.h"
#endif

namespace Glade {
/*

*/
class ParticleNonstiffRodContactGenerator : public ParticleRodContactGenerator
{
public:
	ParticleNonstiffRodContactGenerator(Particle* p1_, Particle* p2_, gFloat len, gFloat stiff, gFloat tear);

	unsigned int GenerateContact(ParticleContact* contacts, unsigned int limit) const;	

	Vector GetP1() { return p1->GetPosition(); }
	Vector GetP2() { return p2->GetPosition(); }
protected:
	gFloat stiffness;
	gFloat tearSensitivity;
};
}	// namespace
#endif	// GLADE_PARTICLE_CLOTH_LINK_CONTACT_GENERATOR


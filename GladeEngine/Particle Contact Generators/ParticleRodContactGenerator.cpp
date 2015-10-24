#include "ParticleRodContactGenerator.h"

using namespace Glade;

ParticleRodContactGenerator::ParticleRodContactGenerator(Particle* p1_, Particle* p2_, gFloat len) : length(len), ParticleLinkContactGenerator(p1_, p2_)
{
}


unsigned int ParticleRodContactGenerator::GenerateContact(ParticleContact* contacts, unsigned int limit) const
{
	gFloat currentLength = CalcCurrentLength();

	// Rod keeps 2 Particles at an exact length only
	// Return no contact.
	if(currentLength == length) return 0;

	// Generate Contact to pull/push Particles apart/together
	Vector normal = (p2->GetPosition() - p1->GetPosition()).Normalized();
	gFloat pen = currentLength - length;
	gFloat res = 0;	// Always 0. No bounciness with rods

	// Normal/Penetration flipped if particles are too close
	if(currentLength < length)
	{
		normal*= -1;
		pen = length - currentLength;
	}

	contacts->SetNewContact(p1, p2, res, normal, pen);
	return 1;
}
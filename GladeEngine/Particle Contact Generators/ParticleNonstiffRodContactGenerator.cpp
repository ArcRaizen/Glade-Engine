#include "ParticleNonstiffRodContactGenerator.h"

using namespace Glade;

ParticleNonstiffRodContactGenerator::ParticleNonstiffRodContactGenerator(Particle* p1_, Particle* p2_, gFloat len, gFloat stiff, gFloat tear) : ParticleRodContactGenerator(p1_, p2_, len),
																																		stiffness(stiff), tearSensitivity(tear)
{
}


unsigned int ParticleNonstiffRodContactGenerator::GenerateContact(ParticleContact* contacts, unsigned int limit) const
{
	gFloat currentLength = CalcCurrentLength();

	// Rod keeps 2 Particles at an exact length only
	// Return no contact
	if(currentLength == length) return 0;

	// Generate Contact to push/pull Particles apart/together
	Vector normal = (p2->GetPosition() - p1->GetPosition()).Normalized();
	gFloat pen = currentLength - length;

	if(currentLength > tearSensitivity) return -1;

	// Normal/Penetration flipped if particles are too close
	if(currentLength < length)
	{
		normal *= -1;
		pen = length - currentLength;
	}

	contacts->SetNewContact(p1, p2, 1-stiffness, normal, pen);
	return 1;
}
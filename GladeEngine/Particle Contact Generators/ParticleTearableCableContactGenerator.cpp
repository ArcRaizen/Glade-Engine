#include "ParticleTearableCableContactGenerator.h"

using namespace Glade;

ParticleTearableCableContactGenerator::ParticleTearableCableContactGenerator(Particle* p1_, Particle* p2_, gFloat len, gFloat res, gFloat tearLen) : ParticleCableContactGenerator(p1_, p2_, len, res),
																																					tearLength(tearLen)
{
}

unsigned int ParticleTearableCableContactGenerator::GenerateContact(ParticleContact* contacts, unsigned int limit) const
{
//	gFloat currentLength = CalcCurrentLength();
	gFloat currentLength = AppoximateSquareRoot(CalcCurrentSquaredLength(), maxLength, 3);

	// Cable keeps 2 Particles within a maximum length, but does nothing if they are
	// within that legnth. Return no contact.
	if(currentLength <= maxLength) return 0;

	// Cable will tear/break if it is forcibly extended past a certain length
	// Return -1 to delete this Contact Generator once it is torn.
	if(currentLength > tearLength) return -1;

	// Generate Contact to pull Particles together now that they've become too far apart
	contacts->SetNewContact(p1, p2, coeffRestitution, (p2->GetPosition() - p1->GetPosition()).Normalized(), currentLength - maxLength);
	return 1;
}
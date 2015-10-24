#include "ParticleCableContactGenerator.h"

using namespace Glade;

ParticleCableContactGenerator::ParticleCableContactGenerator(Particle* p1_, Particle* p2_, gFloat len, gFloat res) : maxLength(len), coeffRestitution(res), ParticleLinkContactGenerator(p1_, p2_)
{
}

unsigned int ParticleCableContactGenerator::GenerateContact(ParticleContact* contacts, unsigned int limit) const
{
	gFloat currentLength = CalcCurrentLength();

	// Cable keeps 2 Particles within a maximum length, but does nothing if they are
	// within that legnth. Return no contact.
	if(currentLength < maxLength) return 0;

	// Generate Contact to pull Particles together now that they've become too far apart
	contacts->SetNewContact(p1, p2, coeffRestitution, (p2->GetPosition() - p1->GetPosition()).Normalized(), currentLength - maxLength);
	return 1;
}
#include "ParticlePinContactGenerator.h"

using namespace Glade;

ParticlePinContactGenerator::ParticlePinContactGenerator(Particle* p1_, Vector p) : p1(p1_), pin(p)
{
}

unsigned int ParticlePinContactGenerator::GenerateContact(ParticleContact* contacts, unsigned int limit) const
{
	if(p1->GetPosition() == pin) return 0;

	contacts->SetNewContact(p1, nullptr, 1, (pin-p1->GetPosition()).Normalized(), p1->GetPosition()-pin);
	return 1;
}
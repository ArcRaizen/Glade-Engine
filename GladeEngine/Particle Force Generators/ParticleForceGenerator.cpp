#include "ParticleForceGenerator.h"

using namespace Glade;

int ParticleForceGenerator::ForceGeneratorID = 0;

ParticleForceGenerator::ParticleForceGenerator() : forceGeneratorID(ForceGeneratorID++)
{
}


ParticleForceGenerator::~ParticleForceGenerator()
{
}

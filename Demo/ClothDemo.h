#pragma once

#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Orb.h"

class ClothDemo : public Glade::GApplication
{
public:
	ClothDemo();
	~ClothDemo();

	bool Update(float dt);
	void Render();

private:
	std::vector<Particle*> orbs;
	ParticleWorld* pWorld;
	
	int clothWidth;
	int clothHeight;
	float clothRestLength;
	int clothTearLength;
};


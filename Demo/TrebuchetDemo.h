#pragma once
#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Orb.h"

class TrebuchetDemo : public Glade::GApplication
{
public:
	TrebuchetDemo();
	~TrebuchetDemo();

	bool Update(float dt);
	void Draw();

private:
	std::vector<Particle*> orbs;
	ParticleWorld* pWorld;
};


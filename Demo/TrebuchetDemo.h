#pragma once
#include "App.h"

#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Orb.h"

class TrebuchetDemo : public App
{
public:
	TrebuchetDemo();
	~TrebuchetDemo();

	void Update(float dt);
	void Draw();

private:
	std::vector<Particle*> orbs;
	ParticleWorld* pWorld;
};


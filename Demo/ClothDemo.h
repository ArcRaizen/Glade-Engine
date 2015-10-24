#pragma once
#ifndef APP_H
#include "App.h"
#endif

#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Orb.h"

class ClothDemo : public App
{
public:
	ClothDemo();
	~ClothDemo();

	void Update(float dt);
	void Render();

private:
	std::vector<Particle*> orbs;
	ParticleWorld* pWorld;
	
	int clothWidth;
	int clothHeight;
	float clothRestLength;
	int clothTearLength;
};


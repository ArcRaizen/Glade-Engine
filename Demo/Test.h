#pragma once
#ifndef APP_H
#include "App.h"
#endif

#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Orb.h"

class Test : public App
{
public:
	Test();
	~Test();

	void Update(float dt);
	void Render();

private:
	Orb* orb;
	ParticleWorld* pWorld;
};


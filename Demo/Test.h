#pragma once
#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Orb.h"

class Test : public Glade::GApplication
{
public:
	Test();
	~Test();

	bool Update(float dt);
	void Render();

private:
	Orb* orb;
	ParticleWorld* pWorld;
};


#pragma once
#include "Orb.h"

class BuoyancyDemo : public Glade::GApplication
{
public:
	BuoyancyDemo();
	~BuoyancyDemo();

	bool Update(float dt);
	void Draw();

private:

};


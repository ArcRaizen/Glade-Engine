#pragma once

#include "Core.h"
using namespace Glade;
class Arcball
{
public:
	Arcball(float width, float height);
	~Arcball();

	static Vector		MapToSphere(int x, int y);
	static void			Click(int x, int y);
	static Quaternion	Drag(int x, int y);
	static void			Resize(float w, float h);
};


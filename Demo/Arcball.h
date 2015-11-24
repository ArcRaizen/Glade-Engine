#pragma once

#include "Core.h"
using namespace Glade;
class Arcball
{
public:
	static void			Init(float width, float height);
	static Vector		MapToSphere(int x, int y);
	static void			Click(int x, int y);
	static Quaternion	Drag(int x, int y);
	static void			Resize(float w, float h);

	static Vector startVec;
	static Vector endVec;
	static float adjustWidth;
	static float adjustHeight;
};
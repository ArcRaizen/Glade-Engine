#pragma once
#ifndef GLADE_ARCBALL_H
#define GLADE_ARCBALL_H
#include "../../Math/Quaternion.h"

namespace Glade { 
class Arcball
{
public:
	enum class ArcballType { SPHERICAL, PLANAR };

	Arcball(gFloat width, gFloat height, ArcballType t = ArcballType::SPHERICAL);
	Vector		MapToSphere(int x, int y);
	void		Click(int x, int y);
	Quaternion	Drag(int x, int y);
	void		Resize(gFloat w, gFloat h);
	void		SetType(ArcballType t);

private:
	Vector startVec;
	Vector endVec;
	gFloat adjustWidth;
	gFloat adjustHeight;
	ArcballType type;
};
}	// namespace
#endif	// GLADE_ARCBALL_H
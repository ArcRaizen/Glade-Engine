#pragma once
#ifndef APP_H
#include "App.h"
#endif

#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Orb.h"
#include "DrawableOctree.h"

class OctreeDemo : public App
{
public:
	OctreeDemo();
	~OctreeDemo();

	void Update(float dt);
	void Render();

private:
	std::vector<Particle*>	orbs;
	ParticleWorld* pWorld;
	DrawableOctree*	octree;

	Quaternion cameraOrientation;
	Matrix m;

	bool pausePhysics;
	bool drawOctree;
	int numOrbs;
};


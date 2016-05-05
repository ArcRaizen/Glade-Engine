#pragma once
#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Orb.h"

using namespace Glade;
class OctreeDemo : public GApplication
{
public:
	OctreeDemo();
	~OctreeDemo();

	bool Initialize();
	bool Update(float dt);
	void Render();

private:
	std::vector<Particle*>	orbs;
	ParticleWorld* pWorld;
	Octree*	octree;

	Quaternion cameraOrientation;
	Matrix m;

	bool pausePhysics;
	bool drawOctree;
	int numOrbs;
};


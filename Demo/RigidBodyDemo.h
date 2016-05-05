#pragma once
#include "MouseManager.h"
#include "KeyboardManager.h"
#include "Glade.h"

using namespace Glade;
class RigidBodyDemo : public GApplication
{
public:
	RigidBodyDemo();
	~RigidBodyDemo();

	bool Initialize();
	bool Update(float dt);
	void Render();

private:
	RigidBody* box, *plane;
	RigidBody* box2, *box3;
	PhysicMaterial* boxMaterial, *planeMaterial;
	World* world;
};


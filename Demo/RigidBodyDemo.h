#pragma once
#ifndef APP_H
#include "App.h"
#endif

#include "MouseManager.h"
#include "KeyboardManager.h"

class RigidBodyDemo : public App
{
public:
	RigidBodyDemo();
	~RigidBodyDemo();

	void Update(float dt);
	void Render();

private:
	RigidBody* box, *plane;
	World* world;

	Vector cameraPosition;
	Quaternion cameraOrientation;
	Matrix cameraTransform;
};


#include "TrebuchetDemo.h"


TrebuchetDemo::TrebuchetDemo()
{
	MouseManager::Init();
	KeyboardManager::Init();
	glClearColor(0.9f, 0.95f, 1.0f, 1.0f);

	pWorld = new ParticleWorld(10);

	orbs.push_back(new Orb(Vector(-2, 4,-2), Vector(), 0, 1, true, 0.25f, Vector(0,0,1)));
	//orbs.push_back(new Orb(Vector(-2, -2, 2), Vector(), 0, 1, true, 0.25f, Vector(0,0,1)));
	//orbs.push_back(new Orb(Vector(2, -2,-2), Vector(), 0, 1, true, 0.25f, Vector(0,0,1)));
	//orbs.push_back(new Orb(Vector(2, -2, 2), Vector(), 0, 1, true, 0.25f, Vector(0,0,1)));
	orbs.push_back(new Orb(Vector(-2, 8,-2), Vector(), 0, 1, true, 0.25f, Vector(0,0,1)));
	//orbs.push_back(new Orb(Vector(-2, 2, 2), Vector(), 0, 1, true, 0.25f, Vector(0,0,1)));
	//orbs.push_back(new Orb(Vector(2, 2,-2), Vector(), 0, 1, true, 0.25f, Vector(0,0,1)));
	//orbs.push_back(new Orb(Vector(2, 2, 2), Vector(), 0, 1, true, 0.25f, Vector(0,0,1)));

	for(int i = 0; i < orbs.size(); i++)
		pWorld->AddParticle(orbs[i]);

	pWorld->AddContactGenerator(new ParticleSystemPlaneContactGenerator(orbs, Vector(0, 1, 0), -5, 0.05f));
	pWorld->AddContactGenerator(new ParticleRodContactGenerator(orbs[0], orbs[1], 4));
}


TrebuchetDemo::~TrebuchetDemo()
{
}

void TrebuchetDemo::Update(float dt)
{
	pWorld->PhysicsUpdate(0.032);

	glutPostRedisplay();
}

void TrebuchetDemo::Draw()
{
	Vector pos;
	for(int i = 0; i < orbs.size(); i++)
	{
		pos = orbs[i]->GetPosition();
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glColor3f(1,0,0);
		glutSolidSphere(orbs[i]->GetRadius(), 20, 20);
		glPopMatrix();
	}

	glFlush();
	glutSwapBuffers();
}
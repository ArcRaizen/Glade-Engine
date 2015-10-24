#include "Test.h"


Test::Test()
{
	MouseManager::Init();
	KeyboardManager::Init();
	glClearColor(0.9f, 0.95f, 1.0f, 1.0f);

	pWorld = new ParticleWorld(5);
	srand(time(NULL));

	orb = new Orb(Vector(0,2,0), Vector(0,-0.3f,0), 0.0f, 1, true, 0.5f);
	pWorld->AddParticle(orb);
	pWorld->AddContactGenerator(new ParticlePlaneContactGenerator(orb, Vector(0,1,0), -15, 0.9f, 0.5f));
	pWorld->AddContactGenerator(new ParticlePlaneContactGenerator(orb, Vector(0,-1,0), -15, 1, 0.5f));
}


Test::~Test()
{
}

void Test::Update(float dt)
{
	pWorld->PhysicsUpdate(dt);
	glutPostRedisplay();
}

void Test::Render()
{
	Vector pos = orb->GetPosition();
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glColor3f(0,0,0);
	glutSolidSphere(orb->CalcRadius(), 20, 20);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
}


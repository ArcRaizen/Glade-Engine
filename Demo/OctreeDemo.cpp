#include "OctreeDemo.h"


OctreeDemo::OctreeDemo() : drawOctree(true), pausePhysics(false)
{
	MouseManager::Init();
	KeyboardManager::Init();
	glClearColor(0.9f, 0.95f, 1.0f, 1.0f);

	numOrbs = 20;
	pWorld = new ParticleWorld(numOrbs);
	octree = new DrawableOctree(Octree::OctreeType::DYNAMIC, true, Vector(0,0,0), 20, 20, 20, 1, 1, 1.0f);
	srand(time(NULL));

	for(int i = 0; i < numOrbs; i++)
	{
		orbs.push_back(new Orb(Vector(rand()%19 - 9, rand()%5 - 2, rand()%19 - 9),
							Vector(rand()%10-5, rand()%10-5, rand()%10-5) * 0.1f * static_cast<float>(rand())/static_cast<float>(RAND_MAX),
							0, 1, false, 0.05f, Vector(1,0,0)));
		pWorld->AddParticle(orbs[i]);
		octree->AddElement(orbs[i]);
	}

	pWorld->AddContactGenerator(new ParticleSystemPlaneContactGenerator(orbs, Vector(0,-1,0), -10, 1, 0.05f));
	pWorld->AddContactGenerator(new ParticleSystemPlaneContactGenerator(orbs, Vector(0, 1,0), -10, 1, 0.05f));
	pWorld->AddContactGenerator(new ParticleSystemPlaneContactGenerator(orbs, Vector(1, 0,0), -10, 1, 0.05f));
	pWorld->AddContactGenerator(new ParticleSystemPlaneContactGenerator(orbs, Vector(-1,0,0), -10, 1, 0.05f));
	pWorld->AddContactGenerator(new ParticleSystemPlaneContactGenerator(orbs, Vector(0,0, 1), -10, 1, 0.05f));
	pWorld->AddContactGenerator(new ParticleSystemPlaneContactGenerator(orbs, Vector(0,0,-1), -10, 1, 0.05f));
}


OctreeDemo::~OctreeDemo()
{
}

void OctreeDemo::Update(float dt)
{
	if(KeyboardManager::IsKeyPressed('a'))
		pausePhysics = !pausePhysics;
	if(KeyboardManager::IsKeyPressed(' '))
		drawOctree = !drawOctree;

	// Update Particle World and Octree
	if(!pausePhysics)
		pWorld->PhysicsUpdate(PHYSICS_TIMESTEP);
	octree->Update(drawOctree);

	// Update Camera
	if(MouseManager::GetIsDragging())
	{
		Quaternion q = Arcball::Drag(MouseManager::GetMouseX(), MouseManager::GetMouseY());
		q.w = -q.w;

		cameraOrientation *= q.Normalized();
		cameraOrientation.NormalizeInPlace();
	}

	Matrix m = cameraOrientation.ConvertToMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(m.GetArray());

	glutPostRedisplay();
}

void OctreeDemo::Render()
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

	if(drawOctree)
		octree->Draw();
/*	
	glPushMatrix();
	glMultMatrixf(m.GetArray());
	glutSolidTeapot(2);
	glColor3f(1,0,0);
	glPopMatrix();
	m.Rotate(0.05, Vector(-1.f/3.f, 2.f/3.f, 2.f/3.f));
*/
	glFlush();
	glutSwapBuffers();
}

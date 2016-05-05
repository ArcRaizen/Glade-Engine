#include "OctreeDemo.h"

using namespace Glade;

OctreeDemo::OctreeDemo()  : drawOctree(true), pausePhysics(false) { }

bool OctreeDemo::Initialize()
{
	if(!GApplication::Initialize())
		return false;

	camera->SetPerspective(45*DEG2RAD, 16.f/9.f, 1.f, 1000.f);
	//camera->SetOrtho(1280/10, 720/10, -500,500);
	camera->LookAt(Vector(10, 20, -50), Vector(10,20,50), Vector(0,1,0));


	numOrbs = 20;
	pWorld = new ParticleWorld(numOrbs);
	octree = new Octree(Octree::OctreeType::DYNAMIC, true, Vector(0,0,0), 20, 20, 20, 1, 1, 1.0f);
	srand(time(NULL));

	for(int i = 0; i < numOrbs; i++)
	{
		orbs.push_back(new Orb(Vector(rand()%19 - 9, rand()%5 - 2, rand()%19 - 9),
							(Vector(rand()%10-5, rand()%10-5, rand()%10-5) * gFloat(0.1f)).operator*(float(static_cast<float>(rand())/static_cast<float>(RAND_MAX))),
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
	return true;
}


OctreeDemo::~OctreeDemo()
{
}

bool OctreeDemo::Update(float dt)
{
	if(input->IsKeyDown(DIK_A))
		pausePhysics = !pausePhysics;
	if(input->IsKeyDown(DIK_SPACE))
		drawOctree = !drawOctree;

	// Update Particle World and Octree
	if(!pausePhysics)
		pWorld->PhysicsUpdate(PHYSICS_TIMESTEP);

	// Update Camera
	if(MouseManager::GetIsDragging())
	{
		Quaternion q;// = Arcball::Drag(MouseManager::GetMouseX(), MouseManager::GetMouseY());
		q.w = -q.w;

		cameraOrientation *= q.Normalized();
		cameraOrientation.NormalizeInPlace();
	}

/*
	Matrix m = cameraOrientation.ConvertToMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf((float*)m);
	glutPostRedisplay();
*/
	return true;
}

void OctreeDemo::Render()
{
	GraphicsLocator::GetGraphics()->StartFrame(100.f/256.f,149.f/256.f,237.f/256.f,1, camera->GetView(), camera->GetProj());
	for(unsigned int i = 0; i < orbs.size(); ++i)
		orbs[i]->Render();

//	if(drawOctree)
//		octree->Draw();

	GraphicsLocator::GetGraphics()->EndFrame();

/*	Vector pos;
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
	
//	glPushMatrix();
//	glMultMatrixf(m.GetArray());
//	glutSolidTeapot(2);
//	glColor3f(1,0,0);
//	glPopMatrix();
//	m.Rotate(0.05, Vector(-1.f/3.f, 2.f/3.f, 2.f/3.f));

	glFlush();
	glutSwapBuffers();
*/
}

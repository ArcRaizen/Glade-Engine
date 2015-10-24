#include "ClothDemo.h"


ClothDemo::ClothDemo()
{
	MouseManager::Init();
	KeyboardManager::Init();
	glClearColor(0.9f, 0.95f, 1.0f, 1.0f);


	clothWidth = 10;
	clothHeight = 10;
	clothRestLength = 1.0f;
	clothTearLength = 2000.0f;
	pWorld = new ParticleWorld(clothHeight * clothWidth*3);
	srand(time(NULL));

	ParticleForceGenerator* pfg = new ParticleAttractorForce(Vector(315,0,0), Vector(-50,-50,5));

	int xStart = -1 * clothWidth * clothRestLength / 2;
	int yStart = clothHeight * clothRestLength / 2;
	for(int y = 0; y < clothHeight; y++)
	{
		for(int x = 0; x < clothWidth; x++)
		{
			Vector pos(xStart + (x*clothRestLength), yStart - (y*clothRestLength), 0);
			orbs.push_back(new Orb(pos, Vector(), 0.3f, y==0?0:1.0f/5.0f, true, 0.1f));
			orbs.back()->RegisterForceGenerator(pfg->GetID());
			pWorld->AddParticle(orbs.back());

			if(x)
			{
				Particle* xLink1 = orbs[y*clothWidth+x], *xLink2 = orbs[y*clothWidth+x-1];
				//pWorld->AddContactGenerator(new ParticleNonstiffRodContactGenerator(orbs[y*clothWidth+x], orbs[y*clothWidth+x-1], clothRestLength, 1, clothTearLength));
				pWorld->AddContactGenerator(new ParticleTearableCableContactGenerator(orbs[y*clothWidth+x], orbs[y*clothWidth+x-1], clothRestLength, 0, clothTearLength));
			}
			if(y)
			{
				Particle* yLink1 = orbs[y*clothWidth+x], *yLink2 = orbs[(y-1)*clothWidth+x];
				//pWorld->AddContactGenerator(new ParticleNonstiffRodContactGenerator(orbs[y*clothWidth+x], orbs[(y-1)*clothWidth+x], clothRestLength, 1, clothTearLength));
				pWorld->AddContactGenerator(new ParticleTearableCableContactGenerator(orbs[y*clothWidth+x], orbs[(y-1)*clothWidth+x], clothRestLength, 0, clothTearLength));
			}
			if(y == 0)
				pWorld->AddContactGenerator(new ParticlePinContactGenerator(orbs[y*clothWidth+x], orbs[y*clothWidth+x]->GetPosition()));
		}
	}

	pWorld->AddForceGenerator(pfg);
	pWorld->AddContactGenerator(new ParticleSystemPlaneContactGenerator(orbs, Vector(0,1,0), -15, 0, 0.1f));
	//pWorld->AddContactGenerator(new ParticleSystemPlaneContactGenerator(orbs, Vector(1,0,0), -12.5, 0, 0.1f));
	//pWorld->AddContactGenerator(new ParticleSystemPlaneContactGenerator(orbs, Vector(-1,0,0), -12.5, 0, 0.1f));
}


ClothDemo::~ClothDemo()
{
}

void ClothDemo::Update(float dt)
{
	pWorld->PhysicsUpdate(dt);
	glutPostRedisplay();
}

void ClothDemo::Render()
{
	Vector pos;
	std::vector<ParticleContactGenerator*> generators = pWorld->GetContactGenerators();
	for(int y = 0; y < clothHeight; y++)
	{
		for(int x = 0; x < clothWidth; x++)
		{
			pos = orbs[y*clothWidth + x]->GetPosition();
			glPushMatrix();
			glTranslatef(pos.x, pos.y, pos.z);
			glColor3f(0,0,0);
			glutSolidSphere(orbs[y*clothWidth + x]->GetRadius(), 20, 20);
			glPopMatrix();
		}
	}

	ParticleTearableCableContactGenerator* g;
	for(auto iter = generators.begin(); iter != generators.end(); iter++)
	{
		g = dynamic_cast<ParticleTearableCableContactGenerator*>(*iter);
		if(g)
		{
			glBegin(GL_LINE_STRIP);
			pos = g->GetP1();
			glVertex3f(pos.x, pos.y, pos.z);
			pos = g->GetP2();
			glVertex3f(pos.x, pos.y, pos.z);
			glEnd();
		}
	}
	
	glFlush();
	glutSwapBuffers();
}
#include "RigidBodyDemo.h"


RigidBodyDemo::RigidBodyDemo()
{
	MouseManager::Init();
	KeyboardManager::Init();
	glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
	srand(time(NULL));

	box = new RigidBody(Vector(10,50,10), Quaternion(0,PI/4,0), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), 1, 1, 0.2f, true, Vector::GRAVITY);
	box->AddCollider(new BoxCollider(box, Vector(2,2,2)));
	box->SetInverseInertiaTensor(Matrix::CuboidInverseInertiaTensor(5, Vector(2,2,2)));

	plane = new RigidBody(Vector(10,10,10), Quaternion(0,0,0,1), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), 1, 1, 0.00f, false, Vector::GRAVITY);
	plane->AddCollider(new BoxCollider(plane, Vector(10,1,10)));
	plane->SetInverseInertiaTensor(Matrix::INFINITE_MASS_INERTIA_TENSOR/*Matrix::CuboidInverseInertiaTensor(20.0f, Vector(10, 1, 10))*/);

	world = new World(100);
	world->AddRigidBody(box);
	world->AddRigidBody(plane);

	cameraPosition.x -= 10;
	cameraPosition.y -= 25;
}


RigidBodyDemo::~RigidBodyDemo()
{
}

void RigidBodyDemo::Update(float dt)
{
	world->PhysicsUpdate(dt);

	cameraTransform.ComposeTransformationMatrix(&cameraPosition, &cameraOrientation, nullptr);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
	glMultMatrixf(cameraTransform.GetArray());
}

void RigidBodyDemo::Render()
{
	// Draw box
	Vector pos = box->GetPosition();
	std::vector<Collider*> c;
	box->GetColliders(c);
	Matrix m = c[0]->GetTransform();
	{
	Vector vertices[8] = {
		Vector(2,2,2),
		Vector(2,-2,2),
		Vector(-2,-2,2),
		Vector(-2,2,2),
		Vector(2,2,-2),
		Vector(2,-2,-2),
		Vector(-2,-2,-2),
		Vector(-2,2,-2),
	};
	for(unsigned int i = 0; i < 8; ++i)
		vertices[i] *= m;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1, 0, 0);	// Front Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
		glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
	glEnd();
	glColor3f(0, 1, 0);	// Left Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
		glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
		glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
		glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
	glEnd();
	glColor3f(0, 0, 1);	// Right Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
	glEnd();
	glColor3f(1, 1, 1);	// Top Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
		glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
	glEnd();
	glColor3f(1, 0, 1);	// Bottom Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
		glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
	glEnd();
	glColor3f(0, 1, 1);	// Back Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
		glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
		glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
	glEnd();
	}

	pos = plane->GetPosition();
	std::vector<Collider*> c2;
	plane->GetColliders(c2);
	m = c2[0]->GetTransform();
	Vector vertices[8] = {
		Vector(10,1,10),
		Vector(10,-1,10),
		Vector(-10,-1,10),
		Vector(-10,1,10),
		Vector(10,1,-10),
		Vector(10,-1,-10),
		Vector(-10,-1,-10),
		Vector(-10,1,-10),
	};
	for(unsigned int i = 0; i < 8; ++i)
		vertices[i] *= m;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
	glColor3f(1,0,0);	// Front Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
		glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
	glEnd();
	glColor3f(0, 1, 0);	// Left Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
		glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
		glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
		glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
	glEnd();
	glColor3f(0, 0, 1);	// Right Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
	glEnd();
	glColor3f(1, 1, 1);	// Top Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
		glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
	glEnd();
	glColor3f(1, 0, 1);	// Bottom Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
		glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
	glEnd();
	glColor3f(0, 1, 1);	// Back Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
		glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
		glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
	glEnd();
	glFlush();
	glutSwapBuffers();
}
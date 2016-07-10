#include "RigidBodyDemo.h"
#include <iostream>
#include <stdio.h>

RigidBodyDemo::RigidBodyDemo() { }

bool RigidBodyDemo::Initialize()
{
	if(!GApplication::Initialize())
		return false;
	input->InitArcball();
	baseView = camera->GetView();
	ortho = Matrix::Ortho(1280, 720, 1, 1000);

	typedef PhysicMaterial::PhysicMaterialCombine MaterialCombine;
	auto boxMaterial = PhysicMaterial::CreateFromData(std::string("Box Material"), MaterialCombine::GEOMETRIC_AVERAGE, MaterialCombine::PYTHAGOREAN, 
		0.4f, 0.1f, 0.1f);
	auto planeMaterial = PhysicMaterial::CreateFromData(std::string("Plane Material"), MaterialCombine::GEOMETRIC_AVERAGE, MaterialCombine::PYTHAGOREAN,
		0.4f, 0.5f, 0.4f);

	MeshData* boxMesh = GeometryGenerator::CreateBox(4, 4, 4, "box.png");
	MeshData* cylinderMesh = GeometryGenerator::CreateCylinder(2, 5, 16, 1, "checkerboardsmall.png");
	MeshData* planeMesh = GeometryGenerator::CreateBox(20, 2, 20, "box.png");

	gFloat boxInvMass = 0.2f;
	gFloat planeInvMass = 0.00f;
	box = new RigidBody(Vector(-3,12.5,10), Quaternion(0,0,-00*DEG2RAD), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), 0.95f, 0.8f, true, Vector::GRAVITY);
	box->AddCollider(new BoxCollider(box, PhysicMaterial::FindResourceByName("Box Material"), boxInvMass, Vector(2,2,2)));
	//box->AddCollider(new SphereCollider(box, boxInvMass, 2));
	box->LoadMesh(boxMesh);
	box->AllowSetVelocity();
	box->SetAwake(true);

	box2 = new RigidBody(Vector(-2.9,50,10), Quaternion(0,0,0), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), 0.95f, 0.8f, true, Vector::GRAVITY);
	box2->AddCollider(new BoxCollider(box2, boxMaterial, boxInvMass, Vector(2,2,2)));
	box2->LoadMesh(boxMesh);
	box2->SetAwake(false);

	box3 = new RigidBody(Vector(10,20.7,10), Quaternion(0,0,0), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), 0.95f, 0.8f, true, Vector::GRAVITY);
	//box3->AddCollider(new BoxCollider(box3, boxMaterial, boxInvMass, Vector(2,2,2)));
	box3->AddCollider(new CylinderCollider(box3, boxMaterial, boxInvMass, 2, 5));
	//box3->LoadMesh(boxMesh);
	box3->LoadMesh(cylinderMesh);
	box3->SetAwake(false);

	plane = new RigidBody(Vector(3,10,10), Quaternion(0,0,0), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), Vector(0,0,0), 1, 1, true, Vector::GRAVITY);
	plane->AddCollider(new BoxCollider(plane, planeMaterial, planeInvMass, Vector(10,1,10)));
	plane->LoadMesh(planeMesh);
	//plane->SetInverseInertiaTensor(Matrix::INFINITE_MASS_INERTIA_TENSOR/*Matrix::CuboidInverseInertiaTensor(1.0f/planeInvMass, Vector(20, 2, 20))*/);

	world = new World(-100, 100, 10, 100);
	world->AddRigidBody(box);
	world->AddRigidBody(box2);
	world->AddRigidBody(box3);
	world->AddRigidBody(plane);

	delete boxMesh;
	delete cylinderMesh;
	delete planeMesh;

//	camera->SetWatchTarget(box, Vector(0,0,0));

	GraphicsLocator::GetGraphics()->SetRasterizerState(Direct3D::RasterState::SOLID);
	GraphicsLocator::GetGraphics()->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return true;
}


RigidBodyDemo::~RigidBodyDemo()
{
}

bool RigidBodyDemo::Update(float dt)
{
	if(input->IsKeyDown(GK_SPACE) ||
		input->CheckKeyDownEvent(GK_Z))
		world->PhysicsUpdate(dt);

	camera->StrafeWorld(input->GetAxis("Horizontal") * 30*dt);
	camera->WalkWorld(input->GetAxis("Vertical") * 30*dt);
	camera->ClimbWorld(input->GetAxis("Forward") * 30*dt);
	camera->PitchWorld(input->GetAxis("Pitch") * -30*DEG2RAD*dt);
	camera->TurnWorld(input->GetAxis("Yaw") * 30*DEG2RAD*dt);
	camera->RollWorld(input->GetAxis("Roll") * -30*DEG2RAD*dt);

	if(input->IsMouseDragging(0))
	{
		Quaternion q = input->GetArcballQuat();
		q.w = -q.w;
		camera->Rotate(q.Normalized());
		//camera->SetOrientation(q);
	}
	camera->Update(dt);

	std::string s = std::to_string(camera->GetView()(3,0));
	int i = s.length();
	char* test = new char[i+1];
	strcpy_s(test, i+1, s.c_str());
	GraphicsLocator::GetFontGraphics()->PushSentence("test", test, 20, 0, 1, 0, 0);
	delete [] test;

	Ray ray = input->GetMousePickRay(hWnd, camera->GetView(), camera->GetProj());
	gFloat t;
	Object* o = world->RayCast(ray, t, 1);
	if(o != nullptr)
		o->SetHightlightColor(D3DXVECTOR4(0,0,1,1));

/*	static gFloat force = 20;
	if(input->IsKeyDown(DIK_UPARROW))
		force += 1;
	else if(input->IsKeyDown(DIK_DOWNARROW))
		force -= 1;
*/	if(input->CheckKeyDownEvent(DIK_Q))
	{
		box->SetVelocity(Vector(-200, 0,0));
		box->SetAwake(true);
	}

	// TODO - Camera Arcball functionality

	return true;
}

void RigidBodyDemo::Render()
{
	GraphicsLocator::GetGraphics()->StartFrame((float*)camera->GetView(), (float*)camera->GetProj());
	world->Render(camera);
	GraphicsLocator::GetDebugGraphics()->Render(camera->GetView(), camera->GetProj());
	GraphicsLocator::GetGraphics()->DisableZBuffer();
	GraphicsLocator::GetFontGraphics()->Render(baseView, ortho);
	GraphicsLocator::GetGraphics()->EnableZBuffer();
	GraphicsLocator::GetGraphics()->EndFrame();
}

/*
bool RigidBodyDemo::Update(float dt)
{
	if(KeyboardManager::IsKeyPressed(32) ||
		KeyboardManager::WasKeyPressedThisFrame('a'))
		world->PhysicsUpdate(dt);
	if(KeyboardManager::IsKeyPressed(ARROW_KEY_LEFT))
		cameraPosition.x += 1;
	else if(KeyboardManager::IsKeyPressed(ARROW_KEY_RIGHT))
		cameraPosition.x -= 1;
//	if(KeyboardManager::IsKeyPressed(ARROW_KEY_UP))
//		cameraPosition.y -= 1;
//	else if(KeyboardManager::IsKeyPressed(ARROW_KEY_DOWN))
//		cameraPosition.y += 1;

	static gFloat force = 20;
	if(KeyboardManager::IsKeyPressed(ARROW_KEY_UP))
		force += 1;
	else if(KeyboardManager::IsKeyPressed(ARROW_KEY_DOWN))
		force -= 1;
	if(KeyboardManager::WasKeyPressedThisFrame('q'))
	{
		box->SetVelocity(Vector(force,0,0));
		box->SetAwake(true);
	}

	// Update Camera
	if(MouseManager::GetIsDragging())
	{
		Quaternion q = Arcball::Drag(MouseManager::GetMouseX(), MouseManager::GetMouseY());
		q.w = -q.w;
		
		cameraOrientation *= q.Normalized();
		cameraOrientation.NormalizeInPlace();
	}

	cameraTransform.ComposeTransformationMatrix(&cameraPosition, &cameraOrientation, nullptr);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(cameraTransform.GetArray());
	glutPostRedisplay();
	return true;
}

void RigidBodyDemo::Render()
{
	// Draw box
	Vector pos = box->GetPosition();
	Quaternion o = box->GetOrientation();
	Matrix m(&pos, &o, nullptr);
//	std::vector<Collider*> c;
//	box->GetColliders(c);
//	Matrix m = c[0]->GetTransform();
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
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 1, 0);	// Left Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 0, 1);	// Right Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
	glEnd();
	glColor3f(1, 1, 0);	// Top Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
	glEnd();
	glColor3f(1, 0, 1);	// Bottom Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 1, 1);	// Back Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
	glEnd();
	}

// ~~~~~~~~ BOX 2 ~~~~~~~~
	pos = box2->GetPosition();
	o = box2->GetOrientation();
	m.ComposeTransformationMatrix(&pos, &o, nullptr);
//	std::vector<Collider*> c;
//	box->GetColliders(c);
//	Matrix m = c[0]->GetTransform();
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
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 1, 0);	// Left Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 0, 1);	// Right Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
	glEnd();
	glColor3f(1, 1, 0);	// Top Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
	glEnd();
	glColor3f(1, 0, 1);	// Bottom Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 1, 0);	// Back Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
	glEnd();
	}

// ~~~~~~~~ BOX 3 ~~~~~~~~
	pos = box3->GetPosition();
	o = box3->GetOrientation();
	m.ComposeTransformationMatrix(&pos, &o, nullptr);
//	std::vector<Collider*> c;
//	box->GetColliders(c);
//	Matrix m = c[0]->GetTransform();
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
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 1, 0);	// Left Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 0, 1);	// Right Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
	glEnd();
	glColor3f(1, 1, 0);	// Top Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
	glEnd();
	glColor3f(1, 0, 1);	// Bottom Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(1, 0, 1);	// Back Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
	glEnd();
	}
// ~~~~ DEBUG WRITE BOX STATS TO SCREEN
	glPushMatrix();
	glLoadIdentity();
	char text[100];
	Vector position = box->GetPosition();
	Vector vel = box->GetVelocity();
	Vector angVel = box->GetAngularVelocity();
	Quaternion orient = box->GetOrientation();
	Vector euler;
	orient.EulerAngles(euler.x, euler.y, euler.z);
	euler *= RAD2DEG;

	sprintf_s(text, "Position = (%f, %f, %f)", position.x, position.y, position.z);
	glColor3f(0.0,0.0,0.0);
	glRasterPos2f(-24.0f, 23.0f);
	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, text[i]);

	sprintf_s(text, "Velocity = (%f, %f, %f)", vel.x, vel.y, vel.z);
	glColor3f(0.0,0.0,0.0);
	glRasterPos2f(-24.0f, 22.0f);
	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, text[i]);

	sprintf_s(text, "AngVel = (%f, %f, %f)", angVel.x, angVel.y, angVel.z);
	glColor3f(0.0,0.0,0.0);
	glRasterPos2f(-24.0f, 21.0f);
	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, text[i]);

	sprintf_s(text, "Orientation = (%f, %f, %f, %f)", orient.x, orient.y, orient.z, orient.w);
	glColor3f(0.0,0.0,0.0);
	glRasterPos2f(-24.0f, 20.0f);
	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, text[i]);

	sprintf_s(text, "Euler Angles = (%f, %f, %f)", euler.x, euler.y, euler.z);
	glColor3f(0.0,0.0,0.0);
	glRasterPos2f(-24.0f, 19.0f);
	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, text[i]);

	glPopMatrix();
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	pos = plane->GetPosition();
	o = plane->GetOrientation();
	m.ComposeTransformationMatrix(&pos, &o, nullptr);
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
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 1, 0);	// Left Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 0, 1);	// Right Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
	glEnd();
	glColor3f(0,0,0);	// Top Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[3].x, vertices[3].y, -vertices[3].z);
		glVertex3f(vertices[0].x, vertices[0].y, -vertices[0].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
	glEnd();
	glColor3f(1, 0, 1);	// Bottom Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[1].x, vertices[1].y, -vertices[1].z);
		glVertex3f(vertices[2].x, vertices[2].y, -vertices[2].z);
	glEnd();
	glColor3f(0, 1, 1);	// Back Face
	glBegin(GL_POLYGON);
		glVertex3f(vertices[7].x, vertices[7].y, -vertices[7].z);
		glVertex3f(vertices[4].x, vertices[4].y, -vertices[4].z);
		glVertex3f(vertices[5].x, vertices[5].y, -vertices[5].z);
		glVertex3f(vertices[6].x, vertices[6].y, -vertices[6].z);
	glEnd();
	glFlush();
	glutSwapBuffers();
}
*/
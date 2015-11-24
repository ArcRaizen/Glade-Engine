#pragma once
#include "OctreeDemo.h"
#include "TrebuchetDemo.h"
#include "ClothDemo.h"
#include "Test.h"
#include "RigidBodyDemo.h"
#include "Arcball.h"
#ifndef KEYBOARD_H
#include "KeyboardManager.h"
#endif
#ifndef MOUSE_H
#include "MouseManager.h"
#endif
#ifndef TIMER_H
#include "TimerManager.h"
#endif
#ifndef CORE_H
#include "Core.h"
#endif
#include <iostream>
#include <algorithm>
using namespace std;

int window;
App* app;
char text[30];
void Init()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-WINDOW_WIDTH/WINDOW_HEIGHT, WINDOW_WIDTH/WINDOW_HEIGHT, -1.0f, 1.0f, -1.0f, 1.0f);

/*	Quaternion q1(0,0,0,1), q2(0,0,0,1);
	Quaternion angVel(PI * (1.0f/60.0f) * 0.5f, PI * (1.0f/60.0f) * 0.5f * -1, 0, 0);
	gFloat x,y,z,a,b,c;
	while(true)
	{
		q1 = q1 + q1*angVel;
		q1.EulerAngles(x,y,z);
		x *= RAD2DEG; y *= RAD2DEG; z *= RAD2DEG;

		q2 = q2 + angVel*q2;
		q2.EulerAngles(a,b,c);
		a *= RAD2DEG; b *= RAD2DEG; c *= RAD2DEG;

		int blah = 5; 
		blah++;
	}
	int x1 = 5;
	x1++;
*/

	Vector x(5,-3,2), y(0, 7, 1);
	Matrix yMat = Matrix::SkewSymmetricMatrix(y);
	Vector z = x.CrossProduct(y);
	Vector z2 = x * yMat;

	TimerManager::Init();
	Arcball::Init(WINDOW_WIDTH, WINDOW_HEIGHT);
	app = new RigidBodyDemo();
	srand(time(NULL));
}

void Resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-25.0, 25.0, -25.0, 25.0, -50.0, 100.0);
//	glFrustum(-25.0, 25.0, -25.0, 25.0, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);

	Arcball::Resize(w, h);
}

void Update()
{
	TimerManager::Update();
	app->Update((float)TimerManager::lastFrameDuration * 0.001f);
	KeyboardManager::Update();

	glPushMatrix();
	glLoadIdentity();
	sprintf_s(text, "FPS: %f", (float)TimerManager::fps);
	glColor3f(0.0,0.0,0.0);
	glRasterPos2f(20.0f, 23.0f);
	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, text[i]);
	glPopMatrix();

	app->Render();
}

void Display()
{
	glutSetWindow(window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-WINDOW_WIDTH/WINDOW_HEIGHT, WINDOW_WIDTH/WINDOW_HEIGHT, -1.0f, 1.0f, -1.0f, 1.0f);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0,0);
	window = glutCreateWindow("DEMO");

	Init();
	glutDisplayFunc(Display);
	glutIdleFunc(Update);
	glutReshapeFunc(Resize);
	glutMouseFunc(MouseManager::MousePress);
	glutMotionFunc(MouseManager::MouseDrag);
	glutPassiveMotionFunc(MouseManager::MouseMovement);
	glutKeyboardFunc(KeyboardManager::KeyDown);
	glutKeyboardUpFunc(KeyboardManager::KeyUp);
	glutSpecialFunc(KeyboardManager::SpecialKeyDown);
	glutSpecialUpFunc(KeyboardManager::SpecialKeyUp);

	glutMainLoop();

	return 0;
}
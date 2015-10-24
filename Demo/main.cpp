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

int IntersectRectQuad2(gFloat h[2], gFloat p[8], gFloat ret[16])
{
	// q and r contain nq and nr coordinate points for current and chopped polygons
	int nq = 4, nr = 0;
	gFloat buffer[16];
	gFloat* q = p;
	gFloat* r = ret;

	for(unsigned int dir = 0; dir <= 1; ++dir)
	{	// direction notation: xy[0] = x-axis, xy[1] = y-axis
		for(int sign = -1; sign <= 1; sign +=2)
		{	// chop q along line xy[dir] = sign*h[dir]
			gFloat* pq = q;
			gFloat* pr = r;
			nr = 0;
			
			for(int i = nq; i > 0; --i)
			{	// go through all points in 'q' and all lines between adjacent points
				if(sign * pq[dir] < h[dir])
				{	// this point is inside the chopping line
					pr[0] = pq[0];
					pr[1] = pq[1];
					pr += 2;
					nr++;
					if(nr & 8)
					{
						q = r;
						goto done;
					}
				}

				gFloat* nextQ = (i > 1) ? pq+2 : q;
				if((sign * pq[dir] < h[dir]) ^ (sign * nextQ[dir] < h[dir]))
				{	// this line crosses the chopping line
					pr[1-dir] = pq[1-dir] + (nextQ[1-dir] - pq[1-dir]) / 
						(nextQ[dir] - pq[dir]) * (sign * h[dir] - pq[dir]);
					pr[dir] = sign * h[dir];
					pr += 2;
					nr++;
					if(nr & 8)
					{
						q = r;
						goto done;
					}
				}
				pq += 2;
			}
			q = r;
			r = (q == ret) ? buffer : ret;
			nq = nr;
		}
	}
done:
	if(q != ret) memcpy(ret, q, nr*2*sizeof(gFloat));
	return nr;
}

int window;
App* app;
Arcball* arcball;
char text[30];
void Init()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-WINDOW_WIDTH/WINDOW_HEIGHT, WINDOW_WIDTH/WINDOW_HEIGHT, -1.0f, 1.0f, -1.0f, 1.0f);

	TimerManager::Init();
	app = new RigidBodyDemo();
	arcball = new Arcball(WINDOW_WIDTH, WINDOW_HEIGHT);
	srand(time(NULL));

	gFloat rect[2] = {5, 3};
	gFloat quad[8] = {2,-1,2,-3,12,-3,12,-1};
	gFloat ret[16];

	int n = IntersectRectQuad2(rect, quad, ret);

	Matrix m1, m2;
	m1.Translate(0,0,0);
	m2.RotateZ(-150*PI/180);
	m2.Translate(1,3.5,0);
	BoxCollider* b1 = new BoxCollider(nullptr, Vector(3,3,3));
	BoxCollider* b2 = new BoxCollider(nullptr, Vector(1,1,1));
	b1->CalcTransformAndDerivedGeometricData(m1);
	b2->CalcTransformAndDerivedGeometricData(m2);

//	CollisionTests::TestCollision(b1, b2, nullptr);


	int x = 5;
	x++;
}

void Resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-25.0, 25.0, -25.0, 25.0, -50.0, 100.0);
	glMatrixMode(GL_MODELVIEW);

	Arcball::Resize(w, h);
}

void Update()
{
	TimerManager::Update();
	app->Update((float)TimerManager::lastFrameDuration * 0.001f);

	sprintf_s(text, "FPS: %f", (float)TimerManager::fps);
	glRasterPos2f(20.0f, 23.0f);
	for(int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, text[i]);

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
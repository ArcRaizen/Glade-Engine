#include "MouseManager.h"

bool MouseManager::isDragging = false;
bool MouseManager::isMousePressed = false;
float MouseManager::mouseX = 0;
float MouseManager::mouseY = 0;

MouseManager::MouseManager()
{
}

MouseManager::~MouseManager()
{
}

void MouseManager::Init()
{
	glutMouseFunc(MouseManager::MousePress);
	glutMotionFunc(MouseManager::MouseDrag);
	glutPassiveMotionFunc(MouseManager::MouseMovement);
}

int MouseManager::GetMouseX()
{
	return mouseX;
}

int MouseManager::GetMouseY()
{
	return mouseY;
}

bool MouseManager::GetIsMousePressed()
{
	return isMousePressed;
}

bool MouseManager::GetIsDragging()
{
	return isDragging;
}

void MouseManager::MousePress(int button, int state, int x, int y)
{

	if(button == 0)
	{
		isMousePressed = true;
		isDragging = false;
		glutSetCursor(GLUT_CURSOR_NONE);
		Arcball::Click(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
	}
	else
	{
		isMousePressed = false;
		isDragging = false;
	}
}

void MouseManager::MouseDrag(int x, int y)
{
	isMousePressed = true;
	isDragging = true;
	mouseX = x;
	mouseY = y;
}

void MouseManager::MouseMovement(int x, int y)
{
	isMousePressed = false;
	isDragging = false;
	mouseX = x;
	mouseY = y;
	glutSetCursor(GLUT_CURSOR_INHERIT);
}

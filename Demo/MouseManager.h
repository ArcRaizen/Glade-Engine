#ifndef MOUSE_H
#define MOUSE_H

#include "Arcball.h"
#ifndef CORE_H
#include "Core.h"
#endif

class MouseManager
{
public:
	MouseManager(void);
	~MouseManager(void);

	static void Init();
	static int  GetMouseX();
	static int  GetMouseY();
	static bool GetIsMousePressed();
	static bool	GetIsDragging();
	static void MousePress(int button, int state, int x, int y);
	static void MouseDrag(int x, int y);
	static void MouseMovement(int x, int y);

	static bool isDragging;
	static bool isMousePressed;
	static float mouseX;
	static float mouseY;

};

#endif
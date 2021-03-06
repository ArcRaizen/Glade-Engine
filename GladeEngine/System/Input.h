#pragma once
#ifndef GLADE_INPUT_H
#define GLADE_INPUT_H

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include "KeyCodes.h"
#include <D3DX10math.h>
#include <map>
#include "Arcball/Arcball.h"
#include "../Math/Ray.h"
#include "../Utils/Assert.h"

namespace Glade {
//#define AXIS_INPUT_SNAP
class Input
{
public:
	enum class Type { KEY_MOUSE_BUTTON, MOUSE_MOVEMENT };
private:
	// Struct to represent an analog input instead of a digital input
	// For KEY_MOUSE_BUTTONS, pressing the 'positive' makes the output move towards 1
	//		and pressing the 'negative' key moves towards -1.
	// The rate of that movement is "acceleration" - the higher, the faster the movement
	// If no buttons pressed, the value returns to 0 at rate of "deceleration" - the higher the faster the movement
	//		Think of acceleration and deceleration as "1/x" with "x being the number of seconds you want it to take to reach max ouput or 0 respectively
	//		An acceleration of 0.2 (or 1/5) means it will take 5 seconds to reach maximum output.
	//		A deceleration of 5 (or 1/(1/5)) means it will take 1/5 of a second to reset to 0 from maximum output
	//		An acceleration of 60 means it will take 1/60 of a second (or 1 frame) to reach maximum output. Anything higher than 60 is basically irrelevant
	struct InputAxis
	{
		InputAxis() : type(Type::KEY_MOUSE_BUTTON), positiveKey(GK_NONE), negativeKey(GK_NONE), 
			altPositiveKey(GK_NONE), altNegativeKey(GK_NONE), value(0),
			acceleration(3), deceleration(3)
		{ }
		InputAxis(Type axisType, unsigned int pKey, unsigned int nKey) :
			type(axisType), positiveKey(pKey), negativeKey(nKey), 
			altPositiveKey(GK_NONE), altNegativeKey(GK_NONE), value(0),
			acceleration(3), deceleration(3)
		{ }

		void SetParameters(gFloat accel, gFloat decel, unsigned int altP, unsigned int altN)
		{
			acceleration = accel;
			deceleration = decel;
			altPositiveKey = altP;
			altNegativeKey = altN;
		}

		Type type;
		unsigned int positiveKey, negativeKey;
		unsigned int altPositiveKey, altNegativeKey;
		gFloat value;

		//gFloat dead;
		gFloat acceleration, deceleration;
	};
public:
	Input();
	~Input();

	bool Initialize(HINSTANCE hInstance, HWND hWnd, int width, int height);
	void InitArcball();
	void Shutdown();
	bool Update(gFloat dt);
	void UpdateAxes(gFloat dt);		// Update values of all Input Axes

	// Functions to check state of input devices
	bool IsKeyDown(unsigned int key);
	bool IsKeyUp(unsigned int key);
	bool CheckKeyDownEvent(unsigned int key);
	bool CheckKeyUpEvent(unsigned int key);

	bool IsMouseButtonDown(unsigned int button);
	bool IsMouseButtonUp(unsigned int button);
	bool CheckMouseButtonDownEvent(unsigned int button);
	bool CheckMouseButtonUpEvent(unsigned int button);

	bool IsMouseDragging(unsigned int button);

	void	AddAxis(std::string name, Type t, unsigned int pKey, unsigned int nKey=GK_NONE);
	void	SetAxisOptionalParameters(std::string name, gFloat accel, gFloat decel, unsigned int altPKey=GK_NONE, unsigned int altNKey=GK_NONE);
	gFloat	GetAxis(std::string name);

	Ray			GetMousePickRay(HWND hWnd, const Matrix& view, const Matrix& proj);
	D3DXVECTOR2 GetMouseClientCoords(HWND hWnd);
	D3DXVECTOR2 GetMouseUV(HWND hWnd);

	Quaternion GetArcballQuat();

private:
	int		screenWidth;
	int		screenHeight;
	int		mouseX, mouseY;

	HWND hWnd;
	IDirectInput8*			diObject;
	IDirectInputDevice8*	diKeyboard;
	IDirectInputDevice8*	diMouse;
	unsigned char			keyState[260];		// Last 4 keys are Mouse Buttons
	unsigned char			prevKeyState[260];	// Last 4 keys are Mouse Buttons
	DIMOUSESTATE			mouseState;
	std::map<std::string, InputAxis> inputAxes;

	Arcball*	arcball;
	Quaternion	arcQuat;
};
}	// namespace
#endif	// GLADE_INPUT_H


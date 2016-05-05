#include "Input.h"

using namespace Glade;

Input::Input() : diObject(0), diKeyboard(0), diMouse(0), arcball(nullptr)
{
}

Input::~Input()
{
	if(arcball != nullptr)
		delete arcball;
}

bool Input::Initialize(HINSTANCE hInstance, HWND hWnd, int width, int height)
{
	this->hWnd = hWnd;
	screenWidth = width;
	screenHeight = height;
	mouseX = mouseY = 0;
	HRESULT hr;

	// Initialize main direct input interface
	if(FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&diObject, NULL)))
		return false;

	// Initialize keyboard interface
	if(FAILED(diObject->CreateDevice(GUID_SysKeyboard, &diKeyboard, NULL)))
		return false;
	if(FAILED(diKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return false;
	if(FAILED(diKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
		return false;

	// Initialize mouse interface
	if(FAILED(diObject->CreateDevice(GUID_SysMouse, &diMouse, NULL)))
		return false;
	if(FAILED(diMouse->SetDataFormat(&c_dfDIMouse)))
		return false;
	if(FAILED(diMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		return false;

	// Acquire Keyboard and Mouse
	diKeyboard->Acquire();
	diMouse->Acquire();
	return true;
}

void Input::InitArcball()
{
	arcball = new Arcball(screenWidth, screenHeight, Arcball::ArcballType::PLANAR);
}

void Input::Shutdown()
{
	if(diMouse)
		diMouse->Release();
	if(diKeyboard)
		diKeyboard->Release();
	if(diObject)
		diObject->Release();
}

bool Input::Update(gFloat dt)
{
	// Save previous state of keyboard to compare later
	memcpy(prevKeyState, keyState, sizeof(unsigned char)*260);

	// Read state of keyboard
	HRESULT hr = diKeyboard->GetDeviceState(sizeof(unsigned char)*256, (LPVOID)&keyState);
	if(FAILED(hr))
	{
		// Regain keyboard if focus was lost or was not acquired
		if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			diKeyboard->Acquire();
		else
			return false;
	}
	
	// Save previous state of mouse buttons
	memcpy(&prevKeyState[256], mouseState.rgbButtons, sizeof(BYTE)*4);

	// Read state of mouse
	hr = diMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
	if(FAILED(hr))
	{
		// Regain mouse if focus was lost or was not acquired
		if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			diMouse->Acquire();
		else
			return false;
	}

	// Copy MouseButton states to keyState
	memcpy(&keyState[256], mouseState.rgbButtons, sizeof(BYTE)*4);

	// Update Input Axes
	UpdateAxes(dt);

	// Update location of mouse cursor based on change of its location
	mouseX += mouseState.lX;
	mouseY += mouseState.lY;
	if(mouseX < 0) mouseX = 0;
	else if(mouseX > screenWidth) mouseX = screenWidth;
	if(mouseY < 0) mouseY = 0;
	else if(mouseY > screenHeight) mouseY = screenHeight;

	if(arcball != nullptr && CheckMouseButtonDownEvent(0))
	{
		D3DXVECTOR2 v = GetMouseClientCoords(hWnd);
		arcball->Click(v.x, v.y);
		//arcball->Click(mouseX, mouseY);
		arcQuat.Identity();
		return true;
	}
	if(arcball != nullptr && IsMouseButtonDown(0))
	{
		D3DXVECTOR2 v = GetMouseClientCoords(hWnd);
		arcQuat = arcball->Drag(v.x, v.y);
		//arcQuat = arcball->Drag(mouseX, mouseY);
	}

	return true;
}

void Input::UpdateAxes(gFloat dt)
{
	for(auto iter = inputAxes.begin(); iter != inputAxes.end(); ++iter)
	{
		bool pos = (iter->second.positiveKey < 260 && IsKeyDown(iter->second.positiveKey)) || 
			(iter->second.altPositiveKey < 260 && IsKeyDown(iter->second.altPositiveKey));
		bool neg = (iter->second.negativeKey < 260 && IsKeyDown(iter->second.negativeKey)) || 
			(iter->second.altNegativeKey < 260 && IsKeyDown(iter->second.altNegativeKey));

		if(pos && neg)			// Both buttons pressed, snap to 0
			iter->second.value = gFloat(0.0f);
		else if(!pos && !neg)	// Neither button pressed, return to 0
		{
			if(iter->second.value == gFloat(0.0f)) 
				continue;
			if(iter->second.value > gFloat(0.0f))
				ClampMin<gFloat>(iter->second.value, 0.0f, iter->second.deceleration * dt);
			else
				ClampMax<gFloat>(iter->second.value, 0.0f, iter->second.deceleration * dt);
		}
		else if(pos && !neg)	// Only Positive pressed, move to 1
			ClampMax<gFloat>(iter->second.value, 1.0f, iter->second.acceleration * dt);
		else					// Only negative pressed, move to -1
			ClampMin<gFloat>(iter->second.value, -1.0f, iter->second.acceleration * dt);
	}
}

// KeyDown, KeyUp, KeyDown this fame, KeyUp this frame checks
bool Input::IsKeyDown(unsigned int key) { return keyState[key] & 0x80; }
bool Input::IsKeyUp(unsigned int key) { return !(keyState[key] & 0x80); }
bool Input::CheckKeyDownEvent(unsigned int key) { return (keyState[key]&0x80) && !(prevKeyState[key]&0x80); }
bool Input::CheckKeyUpEvent(unsigned int key) { return !(keyState[key]&0x80) && (prevKeyState[key]&0x80); }

// MouseButtonDown, MouseButtonUp, MouseButton down this frame, MouseButton up this frame checks
bool Input::IsMouseButtonDown(unsigned int button) { return keyState[256+button] & 0x80; }//{ return mouseState.rgbButtons[button] & 0x80; }
bool Input::IsMouseButtonUp(unsigned int button) { return !(keyState[256+button] & 0x80); }//{ return !(mouseState.rgbButtons[button] & 0x80); }
bool Input::CheckMouseButtonDownEvent(unsigned int button) { return (keyState[256+button]&0x80) && !(prevKeyState[256+button]&0x80); } //{ return (mouseState.rgbButtons[button]&0x80) && !(prevMouseButtons[button]&0x80); }
bool Input::CheckMouseButtonUpEvent(unsigned int button) { return !(keyState[256+button]&0x80) && (prevKeyState[256+button]&0x80); } //{ return !(mouseState.rgbButtons[button]&0x80) && (prevMouseButtons[button]&0x80); }

// Check if mouse button is down, but was not pressed down this frame
// Mostly used for Arcball
bool Input::IsMouseDragging(unsigned int button)
{
	return keyState[256+button]&0x80 && prevKeyState[256+button]&0x80;
	//return mouseState.rgbButtons[button]&0x80 && prevMouseButtons[button]&0x80;
}

void Input::AddAxis(std::string name, Type t, unsigned int pKey, unsigned int nKey)
{
	AssertMsg(pKey <= 260, "Axis key value must be [0,260]");
	AssertMsg(nKey <= 260, "Axis key value must be [0,260]");
	inputAxes[name] = InputAxis(t, pKey, nKey);
}
void Input::SetAxisOptionalParameters(std::string name, gFloat accel, gFloat decel, unsigned int altPKey, unsigned int altNKey)
{
	AssertMsg((inputAxes.find(name) != inputAxes.end()), "No Input Axis with name " + name + ".");
	inputAxes[name].SetParameters(accel, decel, altPKey, altNKey);
}
gFloat Input::GetAxis(std::string name)
{
	AssertMsg((inputAxes.find(name) != inputAxes.end()), "No Input Axis with name " + name + ".");
	return inputAxes[name].value;
}

// Get World Coordinates of the mouse
D3DXVECTOR3 Input::GetMouseWorldCoords(HWND hWnd, D3DXVECTOR3 cameraPos)
{
	// Get client(window) width and height
	RECT clientRect;
	GetClientRect(hWnd, (LPRECT)&clientRect);

	// Get cursor position on screen, convert to client coordinates
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);

	// Convert cursor coords to world coords using camera position
	return D3DXVECTOR3(
		(((cursorPos.x / clientRect.right) * screenWidth) - screenWidth/2) + cameraPos.x, 
		(((cursorPos.y / clientRect.bottom) * screenHeight) + screenHeight/2) + cameraPos.y, 
		0.0f);
}

// Get client coordinates of the mouse
D3DXVECTOR2 Input::GetMouseClientCoords(HWND hWnd)
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);
	return D3DXVECTOR2(cursorPos.x, cursorPos.y);
}

D3DXVECTOR2 Input::GetMouseUV(HWND hWnd)
{
	// Get client(window) width and height
	RECT clientRect;
	GetClientRect(hWnd, (LPRECT)&clientRect);

	// Get cursor position on screen, convert to client coordinates
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);

	// Convert cursor coords to [-1, 1]
	return D3DXVECTOR2(
		((2.0f * cursorPos.x) / clientRect.right) - 1.0f, 
		(((2.0f * cursorPos.y) / clientRect.bottom) -1.0f) * -1.0f);
}

Quaternion Input::GetArcballQuat() { return arcQuat; }
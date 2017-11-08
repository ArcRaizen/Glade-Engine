#pragma once
#ifndef GLADE_APPLICATION_H
#define GLADE_APPLICATION_H
#include "../GladeConfig.h"

#define WIN32_LEAN_AND_MEAN
#include "Graphics\GraphicsLocator.h"
#include "Camera.h"
#include "Input.h"
#include "Clocks\Clock.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

namespace Glade {
class GApplication
{
public:
	GApplication();
	~GApplication();

	virtual bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	virtual bool Update(float dt) = 0;
	virtual void Render() = 0;

protected:
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	LPCSTR applicationName;
	HINSTANCE hInstance;
	HWND hWnd;

	Camera* camera;
	Input* input;
private:
	Direct3D* d3d;
	DebugDraw* dDraw;
	FontDraw* fontDraw;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static GApplication* ApplicationHandle = 0;
}	// namespace
#endif	// GLADE_APPLICATION_H
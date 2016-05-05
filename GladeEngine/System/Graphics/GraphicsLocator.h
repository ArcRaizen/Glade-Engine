#pragma once
#ifndef GLADE_GRAPHICS_LOCATIOR_H
#define GLADE_GRAPHICS_LOCATOR_H
#include <Windows.h>
#include "Direct3D.h"
#include "DebugDraw.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

namespace Glade {

// Service Locator Design Pattern for Graphics handling
class GraphicsLocator
{
	friend class GApplication;
public:
	static Direct3D* GetGraphics() { return service; }
	static DebugDraw* GetDebugGraphics() { return debugService; }

private:
	static void Register(Direct3D* d3d) { service = d3d; }
	static void RegisterDebug(DebugDraw* ds) { debugService = ds; }

	static Direct3D* service;
	static DebugDraw* debugService;
};
}	// namespace Glade
#endif	// GLADE_GRAPHICS_LOCATOR_H

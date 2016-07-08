#include "Application.h"

using namespace Glade;

GApplication::GApplication() : input(0), d3d(0)
{
}

GApplication::~GApplication()
{
}

bool GApplication::Initialize()
{
	int	screenWidth = 0, screenHeight = 0;

	// Initialize the Windows API
	InitializeWindows(screenWidth, screenHeight);

	srand(time(NULL));

	// Create main camera
	camera = new Camera();
	//camera->SetPerspectiveFoVY(45*DEG2RAD, 16.f/9.f, 1.f, 1000.f);
	//camera->SetOrtho(1280/10, 720/10, -500,500);
	camera->LookAt(Vector(10, 20, -50), Vector(10,20,50), Vector(0,1,0));

	// Create input object
	input = new Input();
	if(!input->Initialize(hInstance, hWnd, screenWidth, screenHeight))
		return false;

	// Add default axes
	input->AddAxis("Horizontal", Input::Type::KEY_MOUSE_BUTTON, GK_D, GK_A);
		input->SetAxisOptionalParameters("Horizontal", 3, 3, GK_RIGHT, GK_LEFT);
	input->AddAxis("Vertical", Input::Type::KEY_MOUSE_BUTTON, GK_W, GK_S);
		input->SetAxisOptionalParameters("Vertical", 3, 3, GK_UP, GK_DOWN);
	input->AddAxis("Forward", Input::Type::KEY_MOUSE_BUTTON, GK_E, GK_Q);
		input->SetAxisOptionalParameters("Forward", 3, 3, GK_U, GK_J);
	input->AddAxis("Pitch", Input::Type::KEY_MOUSE_BUTTON, GK_T, GK_G);
	input->AddAxis("Yaw", Input::Type::KEY_MOUSE_BUTTON, GK_H, GK_F);
	input->AddAxis("Roll", Input::Type::KEY_MOUSE_BUTTON, GK_Y, GK_R);

	// Create and Initialize Direct3D object
	d3d = new Direct3D();
	if(!d3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hWnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hWnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}

	// Give graphics object to graphics locator
	GraphicsLocator::Register(d3d);

	dDraw = new DebugDraw();
	if(!dDraw->InitDebugDraw())
	{
		MessageBox(hWnd, "Could not initialize Debug Draw", "Error", MB_OK);
		return false;
	}

	// Give debug graphics to graphics locator
	GraphicsLocator::RegisterDebug(dDraw);

	fontDraw = new FontDraw();
	if(!fontDraw->InitFontDraw(hWnd, screenWidth, screenHeight))
	{
		MessageBox(hWnd, "Could not initialize Font Draw", "Error", MB_OK);
		return false;
	}

	// Give font draw to graphics location
	GraphicsLocator::RegisterFont(fontDraw);

	return true;
}

void GApplication::Shutdown()
{
	// Release Direct3D object
	if(d3d)
	{
		d3d->Shutdown();
		delete d3d;
		d3d = 0;
	}

	if(dDraw)
	{
		dDraw->Shutdown();
		delete dDraw;
		dDraw = 0;
	}

	if(fontDraw)
	{
		fontDraw->Shutdown();
		delete fontDraw;
		fontDraw = 0;
	}

	// Release input
	if(input)
	{
		input->Shutdown();
		delete input;
		input = 0;
	}

	ShutdownWindows();
}

void GApplication::Run()
{
	MSG msg;
	bool done = false;

	// Initialize message structure
	ZeroMemory(&msg, sizeof(MSG));

	// Initialize timer stuff
	gameTimer.Start();

	// Loop until there is a quit message from the window or the user
	gFloat dt;
	while(!done)
	{
		// Handle windows messages
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Quit
		if(msg.message == WM_QUIT)
			done = true;
		else
		{
			gameTimer.Tick();
			gameTimer.DeltaTime(dt);
			input->Update(dt);
			if(!Update(dt))
				done = true;
			Render();
		}
	}
}

LRESULT CALLBACK GApplication::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void GApplication::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get external pointer to this object
	ApplicationHandle = this;

	// Get instance pf this application
	hInstance = GetModuleHandle(NULL);

	// Give the application a name
	applicationName = "Glade Engine Demo";

	// Setup the windows class with default settings
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= MAKEINTRESOURCE(109);
	wc.lpszClassName	= applicationName;
	wc.cbSize			= sizeof(WNDCLASSEX);

	// Register the window class
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode
	if(FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize			= sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight	= (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change display settings to full screen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set position of the window to the top-left corner
		posX = posY = 0;
	}
	else
	{
		// If windowed, set to half screen size
		screenWidth = 1280;
		screenHeight = 720;

		// Place the window in the middle of the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName, 
				WS_OVERLAPPEDWINDOW,
				posX, posY, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);

	// Bring the window up on the screen and set focus
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	// Hide mouse cursor
	ShowCursor(true);
	
	return;
}

void GApplication::ShutdownWindows()
{
	// Show mouse cursor
	ShowCursor(true);

	// Fix display settings if leaving full screen
	if(FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	// Remove window
	DestroyWindow(hWnd);
	hWnd = NULL;

	// Remove application instance
	UnregisterClass(applicationName, hInstance);
	hInstance = NULL;

	// Release pointer to this class
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK Glade::WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		case WM_DESTROY:
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		default:
			return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
	}
}
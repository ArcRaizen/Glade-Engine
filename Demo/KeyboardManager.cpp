#include "KeyboardManager.h"

bool KeyboardManager::keys[128];	// 124 keys + 4 arrow keys
bool KeyboardManager::isPaused = false;

KeyboardManager::KeyboardManager(void)
{
}

KeyboardManager::~KeyboardManager()
{
}

void KeyboardManager::Init()
{
	glutKeyboardFunc(KeyboardManager::KeyDown);
	glutKeyboardUpFunc(KeyboardManager::KeyUp);
	glutSpecialFunc(KeyboardManager::SpecialKeyDown);
	glutSpecialUpFunc(KeyboardManager::SpecialKeyUp);
}

// Is a specific key being pressed down?
bool KeyboardManager::IsKeyPressed(int keyIndex)
{
	if(keyIndex >= 0 && keyIndex < 128)
		return keys[keyIndex];

	// Index is out of range
	return false;
}

void KeyboardManager::KeyUp(unsigned char keyIndex, int x, int y)
{
	keys[keyIndex] = false;
}

void KeyboardManager::KeyDown(unsigned char keyIndex, int x, int y)
{
	keys[keyIndex] = true;

	if(keyIndex == 'p')
		isPaused = !isPaused;
}

void KeyboardManager::SpecialKeyUp(int keyIndex, int x, int y)
{
	if(keyIndex == GLUT_KEY_UP)
		keys[ARROW_KEY_UP] = false;
	else if(keyIndex == GLUT_KEY_DOWN)
		keys[ARROW_KEY_DOWN] = false;
	else if(keyIndex == GLUT_KEY_LEFT)
		keys[ARROW_KEY_LEFT] = false;
	else if(keyIndex == GLUT_KEY_RIGHT)
		keys[ARROW_KEY_RIGHT] = false;
}

void KeyboardManager::SpecialKeyDown(int keyIndex, int x, int y)
{
	if(keyIndex == GLUT_KEY_UP)
		keys[ARROW_KEY_UP] = true;
	else if(keyIndex == GLUT_KEY_DOWN)
		keys[ARROW_KEY_DOWN] = true;
	else if(keyIndex == GLUT_KEY_LEFT)
		keys[ARROW_KEY_LEFT] = true;
	else if(keyIndex == GLUT_KEY_RIGHT)
		keys[ARROW_KEY_RIGHT] = true;
}

bool KeyboardManager::IsPaused()
{
	return isPaused;
}
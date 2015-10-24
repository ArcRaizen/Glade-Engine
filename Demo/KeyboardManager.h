#ifndef KEYBOARD_H
#define KEYBOARD_H

#ifndef CORE_H
#include "Core.h"
#endif

class KeyboardManager
{
public:
	KeyboardManager(void);
	~KeyboardManager(void);

	static void Init();
	static bool IsKeyPressed(int keyIndex);
	static void KeyUp(unsigned char keyIndex, int x, int y);
	static void KeyDown(unsigned char keyIndex, int x, int y);
	static void SpecialKeyUp(int keyIndex, int x, int y);
	static void SpecialKeyDown(int keyIndex, int x, int y);

	static bool IsPaused();

	static bool keys[128];	// 124 keys + 4 arrow keys
	static bool isPaused;

};

#endif
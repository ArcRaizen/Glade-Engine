#pragma once
#ifndef APP_H
#define APP_H

#include "Core.h"

using namespace Glade;
class App
{
public:
	App();
	~App();

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;
};
#endif
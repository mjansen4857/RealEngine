#pragma once
#include "Window.h"
#include "RealTimer.h"

class App {
public:
	App();
	int start();
private:
	Window window;
	RealTimer timer;
	void update();
};
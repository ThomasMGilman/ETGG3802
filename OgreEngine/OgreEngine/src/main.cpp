#include "application.h"
#include <variant>

using namespace OgreEngine;

int main(int argc, char* argv[])
{
	Application app;
	app.initApp();
	app.getRoot()->startRendering();
	app.closeApp();
	return 0;
}
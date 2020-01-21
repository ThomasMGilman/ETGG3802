#include "application.h"

int main(int argc, char* argv[])
{
	Application app;
	app.initApp();
	app.getRoot()->startRendering();
	app.closeApp();
	return 0;
}
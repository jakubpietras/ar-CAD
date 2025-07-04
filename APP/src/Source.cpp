#include "EditorApp.h"

int main()
{
	EditorApp* app = new EditorApp();
	app->Run();
	delete app;
	 
	return 0;
}
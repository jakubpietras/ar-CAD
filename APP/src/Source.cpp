#include "EditorApp.h"

int main(int argc, char** argv)
{
	ar::Log::Init();

	auto app = new EditorApp();
	app->Run();
	delete app;
}
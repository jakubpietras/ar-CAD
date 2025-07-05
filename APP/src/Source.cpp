#include "EditorApp.h"

int main(int argc, char** argv)
{
	ar::Log::Init();
	AR_INFO("Initialized logger");

	auto app = new EditorApp();
	app->Run();
	delete app;
}
#include "EditorApp.h"

int main(int argc, char** argv)
{
	ar::Log::Init();
	AR_CORE_INFO("Initialized logger");

	auto app = new EditorApp();
	app->Run();
	delete app;
}
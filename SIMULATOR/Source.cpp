#include "SimApp.h"

int main(int argc, char** argv)
{
	ar::Log::Init();

	auto app = new SimApp();
	app->Run();
	delete app;
}
#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "Sandbox.h"

int main(int pArgc, char** pArgv)
{
#ifdef _DEBUG
	_CrtMemState memStateInit;
	_CrtMemCheckpoint(&memStateInit);
#endif
	
	Engine::ApplicationSpecification spec;
	spec.Name = "Sandbox";
	const auto app = new Sandbox(spec);

	app->Run();

	delete app;

#ifdef _DEBUG
	_CrtMemState memStateEnd, memStateDiff;
	_CrtMemCheckpoint(&memStateEnd);
	if (_CrtMemDifference(&memStateDiff, &memStateInit, &memStateEnd))
	{
		MessageBoxA(nullptr, "MEMORY LEAKS", "DISCLAIMER", 0);
	}
#endif
}

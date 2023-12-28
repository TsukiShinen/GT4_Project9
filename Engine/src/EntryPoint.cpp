#ifdef _DEBUG
#include <iostream>
#include <crtdbg.h>

static uint64_t s_NbrAlloc;

void* operator new(const size_t pSize) { ++s_NbrAlloc; return malloc(pSize); }
void operator delete(void* pBlock, const size_t pSize) { --s_NbrAlloc; return free(pBlock); }
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

#ifdef _DEBUG
	std::cout << "Nbr of allocation at Initialization : " << s_NbrAlloc << "\n";
#endif

	app->Run();

	delete app;

#ifdef _DEBUG
	_CrtMemState memStateEnd, memStateDiff;
	_CrtMemCheckpoint(&memStateEnd);
	if (_CrtMemDifference(&memStateDiff, &memStateInit, &memStateEnd))
	{
		MessageBoxA(nullptr, "MEMORY LEAKS", "DISCLAIMER", 0);
	}

	std::cout << "Nbr of allocation at End of Application : " << s_NbrAlloc << "\n";
#endif
}

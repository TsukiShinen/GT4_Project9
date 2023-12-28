#pragma once

#include "Sandbox.h"

int main(int pArgc, char** pArgv)
{
	Engine::ApplicationSpecification spec;
	spec.Name = "Sandbox";
	const auto app = new Sandbox(spec);

	app->Run();

	delete app;
}

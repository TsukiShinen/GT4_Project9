#include "Core/Application.h"
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

int main(int argc, char** argv)
{
	ApplicationSpecification spec;
	spec.Name = "Application";
	spec.CommandLineArgs = {argc, argv};
	const auto app = new Application(spec);

	app->Run();

	delete app;
}

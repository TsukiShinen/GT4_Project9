#include "Core/Application.h"

int main(int argc, char** argv)
{
    ApplicationSpecification spec;
    spec.Name = "Application";
    spec.CommandLineArgs = {argc, argv};
    const auto app = new Application(spec);
	
    app->Run();

    delete app;
}

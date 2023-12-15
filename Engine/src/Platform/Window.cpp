#include "Window.h"

#ifdef PLATFORM_WINDOWS
#include "Platform/WindowsWindow.h"
#endif

namespace Engine
{
	Window* Window::Create(const WindowProps& pWindowProps)
	{
#ifdef PLATFORM_WINDOWS
		return new WindowsWindow(pWindowProps);
#else
		throw std::runtime_error("Can't find platform!");
		return nullptr;
#endif
	}
}

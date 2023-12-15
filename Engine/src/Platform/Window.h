#pragma once

#include <functional>
#include <sstream>
#include <string>

#include "Events/Event.h"

namespace Engine
{
	struct WindowProps
	{
		std::string Title = "Owl Application";
		uint32_t Width = 1600;
		uint32_t Height = 900;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void Update() = 0;

		[[nodiscard]] virtual uint32_t GetWidth() const = 0;
		[[nodiscard]] virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& pCallback) = 0;

		virtual void Sleep(unsigned long pMilliseconds) = 0;

		static void ConsoleWrite(const char* pMessage, uint8_t pColour);
		static void ConsoleWriteError(const char* pMessage, uint8_t pColour);
		static const char* GetExecutablePath();

		virtual const char* GetVulkanRequiredExtension() = 0;

		static Window* Create(const WindowProps& pWindowProps = WindowProps());
	};
}

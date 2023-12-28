#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <windows.h>

#include "Events/Event.h"

namespace Engine
{
	struct WindowProps
	{
		std::string Title = "Application";
		uint32_t Width = 1600;
		uint32_t Height = 900;
	};

	class WindowsWindow
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		WindowsWindow(const WindowProps& pWindowProps);
		~WindowsWindow();

		void Initialize(const WindowProps& pWindowProps);
		void Update();

		[[nodiscard]] uint32_t GetWidth() const { return m_Data.Width; }
		[[nodiscard]] uint32_t GetHeight() const { return m_Data.Height; }

		void SetEventCallback(const EventCallbackFn& pCallback) { m_Data.EventCallback = pCallback; }

		void Sleep(const unsigned long pMilliseconds) { ::Sleep(pMilliseconds); }

		HWND GetWindow() const { return m_Window; }

		static void ConsoleWrite(const char* pMessage, uint8_t pColour);
		static void ConsoleWriteError(const char* pMessage, uint8_t pColour);

	private:
		LRESULT ProcessMessages(uint32_t pMessage, WPARAM pWParam, LPARAM pLParam);
		static LRESULT CALLBACK WindowProc(HWND pWindow, uint32_t pMessage, WPARAM pWParam, LPARAM pLParam);

		HINSTANCE m_Instance;
		HWND m_Window;

		CONSOLE_SCREEN_BUFFER_INFO m_StdOutputCsbi;
		CONSOLE_SCREEN_BUFFER_INFO m_ErrOutputCsbi;

		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		friend class Window;
	};
}

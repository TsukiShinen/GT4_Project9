#pragma once

#include <cstdint>
#include <windows.h>

#include "Window.h"

namespace Engine
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& pWindowProps);
		~WindowsWindow() override;

		void Initialize(const WindowProps& pWindowProps);
		void Update() override;

		[[nodiscard]] uint32_t GetWidth() const override { return m_Data.Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Data.Height; }

		void SetEventCallback(const EventCallbackFn& pCallback) override { m_Data.EventCallback = pCallback; }

		void Sleep(const unsigned long pMilliseconds) override { ::Sleep(pMilliseconds); }

		const char* GetVulkanRequiredExtension() override { return "VK_KHR_win32_surface"; }

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

#include "WindowsWindow.h"

#include <stdexcept>
#include <windowsx.h>

#include "Core/Application.h"
#include "Debug/Log.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Platform/Input.h"

namespace Engine
{
	static uint8_t s_WindowCount = 0;

	WindowsWindow::WindowsWindow(const WindowProps& pWindowProps)
	{
		Initialize(pWindowProps);
	}

	WindowsWindow::~WindowsWindow()
	{
		if (m_Window)
		{
			DestroyWindow(m_Window);
			m_Window = nullptr;
		}
		s_WindowCount--;
	}

	void WindowsWindow::Initialize(const WindowProps& pWindowProps)
	{
		m_Data.Title = pWindowProps.Title;
		m_Data.Width = pWindowProps.Width;
		m_Data.Height = pWindowProps.Height;

		m_Instance = GetModuleHandle(nullptr);

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &m_StdOutputCsbi);
		GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &m_ErrOutputCsbi);

		WNDCLASS windowClass = {};
		windowClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = m_Instance;
		windowClass.hIcon = LoadIcon(m_Instance, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = nullptr;
		windowClass.lpszClassName = L"window_class";

		if (!RegisterClass(&windowClass))
		{
			throw std::runtime_error("[PlatformWindows] Failed to register window!");
		}

		uint32_t windowStyle = WS_OVERLAPPEDWINDOW;

		RECT clientRect = {0, 0, m_Data.Width, m_Data.Height};
		AdjustWindowRectEx(&clientRect, windowStyle, FALSE, WS_EX_OVERLAPPEDWINDOW);

		const uint32_t windowX = 100;
		const uint32_t windowY = 100;

		m_Window = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,
			L"window_class",
			std::wstring(m_Data.Title.begin(), m_Data.Title.end()).c_str(),
			windowStyle,
			windowX, windowY,
			clientRect.right - clientRect.left,
			clientRect.bottom - clientRect.top,
			nullptr, nullptr, m_Instance, nullptr);
		if (!m_Window)
			throw std::runtime_error("Failed to create window");
		SetWindowLongPtr(m_Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(m_Window, SW_SHOW);
		s_WindowCount++;

		CORE_INFO("Window initialized successfully : %s (%d, %d)", m_Data.Title.c_str(), m_Data.Width, m_Data.Height);
	}

	void WindowsWindow::Update()
	{
		MSG message;
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	void WindowsWindow::ConsoleWrite(const char* pMessage, const uint8_t pColour)
	{
		const HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		auto t = Application::Get();
		if (Application::Get() && Application::Get()->GetWindow())
		{
			csbi = Application::Get()->GetWindow()->m_StdOutputCsbi;
		}
		else
		{
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		}

		// FATAL,ERROR,WARN,INFO,DEBUG,TRACE
		static uint8_t levels[6] = {64, 4, 6, 2, 1, 8};
		SetConsoleTextAttribute(consoleHandle, levels[pColour]);
		OutputDebugStringA(pMessage);
		const uint64_t length = strlen(pMessage);
		DWORD numberWritten = 0;
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), pMessage, static_cast<DWORD>(length), &numberWritten, 0);

		SetConsoleTextAttribute(consoleHandle, csbi.wAttributes);
	}

	void WindowsWindow::ConsoleWriteError(const char* pMessage, const uint8_t pColour)
	{
		const HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (Application::Get() && Application::Get()->GetWindow())
		{
			csbi = Application::Get()->GetWindow()->m_ErrOutputCsbi;
		}
		else
		{
			GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi);
		}

		// FATAL,ERROR,WARN,INFO,DEBUG,TRACE
		static uint8_t levels[6] = {64, 4, 6, 2, 1, 8};
		SetConsoleTextAttribute(consoleHandle, levels[pColour]);
		OutputDebugStringA(pMessage);
		const uint64_t length = strlen(pMessage);
		DWORD numberWritten = 0;
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), pMessage, static_cast<DWORD>(length), &numberWritten, 0);

		SetConsoleTextAttribute(consoleHandle, csbi.wAttributes);
	}

	LRESULT WindowsWindow::ProcessMessages(const uint32_t pMessage, WPARAM pWParam, LPARAM pLParam)
	{
		switch (pMessage)
		{
		case WM_CLOSE:
			{
				WindowCloseEvent event;
				m_Data.EventCallback(event);

				return 1;
			}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SIZE:
			{
				RECT r;
				GetClientRect(m_Window, &r);
				m_Data.Width = r.right - r.left;
				m_Data.Height = r.bottom - r.top;

				if (!m_Data.EventCallback) break;
				WindowResizeEvent event(m_Data.Width, m_Data.Height);
				m_Data.EventCallback(event);
			}
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			{
				auto key = static_cast<KeyCode>(pWParam);
				if (pWParam == VK_MENU)
				{
					if (GetKeyState(VK_RMENU) & 0x8000)
					{
						key = Key::RightAlt;
					}
					else if (GetKeyState(VK_LMENU) & 0x8000)
					{
						key = Key::LeftAlt;
					}
				}
				else if (pWParam == VK_SHIFT)
				{
					if (GetKeyState(VK_RSHIFT) & 0x8000)
					{
						key = Key::RightShift;
					}
					else if (GetKeyState(VK_LSHIFT) & 0x8000)
					{
						key = Key::LeftShift;
					}
				}
				else if (pWParam == VK_CONTROL)
				{
					if (GetKeyState(VK_RCONTROL) & 0x8000)
					{
						key = Key::RightControl;
					}
					else if (GetKeyState(VK_LCONTROL) & 0x8000)
					{
						key = Key::LeftControl;
					}
				}

				if ((pLParam & (1 << 30)) != 0)
					KeyPressedEvent event(key, true);
				else
				{
					KeyPressedEvent event(key, false);
					m_Data.EventCallback(event);
					Input::s_Keys[key] = true;
				}
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			{
				auto key = static_cast<KeyCode>(pWParam);
				if (pWParam == VK_MENU)
				{
					if (GetKeyState(VK_RMENU) & 0x8000)
					{
						key = Key::RightAlt;
					}
					else if (GetKeyState(VK_LMENU) & 0x8000)
					{
						key = Key::LeftAlt;
					}
				}
				else if (pWParam == VK_SHIFT)
				{
					if (GetKeyState(VK_RSHIFT) & 0x8000)
					{
						key = Key::RightShift;
					}
					else if (GetKeyState(VK_LSHIFT) & 0x8000)
					{
						key = Key::LeftShift;
					}
				}
				else if (pWParam == VK_CONTROL)
				{
					if (GetKeyState(VK_RCONTROL) & 0x8000)
					{
						key = Key::RightControl;
					}
					else if (GetKeyState(VK_LCONTROL) & 0x8000)
					{
						key = Key::LeftControl;
					}
				}

				KeyReleasedEvent event(key);
				m_Data.EventCallback(event);
				Input::s_Keys[key] = false;
			}
			break;
		case WM_CHAR:
			{
				KeyTypedEvent event(static_cast<char>(pWParam));
				m_Data.EventCallback(event);
			}
			break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			{
				MouseButtonPressedEvent event(static_cast<MouseCode>(pWParam));
				m_Data.EventCallback(event);
				Input::s_MouseButtons[static_cast<MouseCode>(pWParam)] = true;
			}
			break;
		case WM_LBUTTONUP:
			{
				MouseButtonReleasedEvent event(1);
				m_Data.EventCallback(event);
				Input::s_MouseButtons[1] = false;
			}
			break;
		case WM_MOUSEWHEEL:
			{
				MouseScrolledEvent event(static_cast<float>(GET_X_LPARAM(pLParam)),
				                         static_cast<float>(GET_Y_LPARAM(pLParam)));
				m_Data.EventCallback(event);
			}
			break;
		case WM_MOUSEMOVE:
			{
				MouseMovedEvent event(static_cast<float>(GET_X_LPARAM(pLParam)),
				                      static_cast<float>(GET_Y_LPARAM(pLParam)));
				m_Data.EventCallback(event);
				Input::s_MousePositionX = event.GetX();
				Input::s_MousePositionY = event.GetY();
			}
			break;
		}

		return DefWindowProc(m_Window, pMessage, pWParam, pLParam);
	}

	LRESULT WindowsWindow::WindowProc(const HWND pWindow, const uint32_t pMessage, WPARAM pWParam, LPARAM pLParam)
	{
		if (auto* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(pWindow, GWLP_USERDATA)))
			return window->ProcessMessages(pMessage, pWParam, pLParam);
		else
			return DefWindowProc(pWindow, pMessage, pWParam, pLParam);
	}
}

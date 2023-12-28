#include "Application.h"

#include "Timestep.h"
#include "Debug/Log.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/DirectXApi.h"
#include "Core/ObjLoader.h"
#include "Renderer/Resource/DirectXResourceManager.h"

#include "Renderer/Shaders/DirectXShader.h"
#include "Renderer/Shaders/DirectXSimpleShader.h"
#include "Renderer/Shaders/DirectXTextureShader.h"
#include "Renderer/Shaders/DirectXLitShader.h"

#include "Renderer/Materials/DirectXMaterial.h"
#include "Renderer/Materials/DirectXSimpleMaterial.h"
#include "Renderer/Materials/DirectXTextureMaterial.h"
#include "Renderer/Materials/DirectXLitMaterial.h"

#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }


namespace Engine
{
	class DirectXCamera;
	
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& pSpecification)
	{
		s_Instance = this;
		m_Window = nullptr;

		// === Window ===
		m_Window = new WindowsWindow(WindowProps(pSpecification.Name, 1280, 700));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		// === Renderer ===
		DirectXApi::Initialize();

		// === Game Timer ===
		__int64 countsPerSec;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSec));
		m_SecondsPerCount = 1.0 / static_cast<double>(countsPerSec);

		__int64 time;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
		m_LastFrameTime = time;
	}

	Application::~Application()
	{
		DirectXApi::Shutdown();
		delete m_Window;
	}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			if (!m_IsMinimized)
			{
				__int64 time;
				QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
				Timestep deltaTime = (time - m_LastFrameTime) * m_SecondsPerCount;
				m_LastFrameTime = time;
				m_Fps = 1.f / deltaTime.GetSeconds();

				Update(deltaTime);

				DirectXApi::BeginFrame();

				Draw();

				DirectXApi::EndFrame();
			}

			m_Window->Update();
		}
	}

	void Application::Update(Timestep pDeltaTime)
	{
		DirectXApi::UpdateCamera(pDeltaTime.GetSeconds());
	}

	void Application::OnEvent(Event& pEvent)
	{
		EventDispatcher dispatcher(pEvent);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<MouseMovedEvent>([](MouseMovedEvent& pEvent)
		{
			DirectXApi::CameraMouseEvent(pEvent.GetX(), pEvent.GetY());
			return true;
		});
	}

	bool Application::OnWindowClose(WindowCloseEvent& pEvent)
	{
		m_IsRunning = false;
		return true;
	}

	bool Application::OnWindowResize(const WindowResizeEvent& pEvent)
	{
		DirectXApi::Resize(pEvent.GetWidth(), pEvent.GetHeight());
		return true;
	}
	
}

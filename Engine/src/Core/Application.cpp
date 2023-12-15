#include "Application.h"

#include "Debug/Log.h"
#include "Events/KeyEvent.h"
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification& pSpecification)
{
    ASSERT(!s_Instance, "Application already exist!")
    s_Instance = this;
	m_Window = nullptr;

    // === Window ===
    m_Window = Engine::Window::Create(Engine::WindowProps(pSpecification.Name, 1280, 700));
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
}

Application::~Application()
{
	delete m_Window;
}

void Application::Run()
{
	while (m_IsRunning)
	{
		if (!m_IsMinimized)
		{
			
		}

		m_Window->Update();
	}
}

void Application::OnEvent(Engine::Event& pEvent)
{
	Engine::EventDispatcher dispatcher(pEvent);
	dispatcher.Dispatch<Engine::WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<Engine::WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
}

bool Application::OnWindowClose(Engine::WindowCloseEvent& pEvent)
{
	m_IsRunning = false;
	return true;
}

bool Application::OnWindowResize(const Engine::WindowResizeEvent& pEvent)
{
	// TODO: Implement with DX12
	return false;
}

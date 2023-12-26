#include "Application.h"

#include "Debug/Log.h"
#include "Events/KeyEvent.h"
#include "Renderer/DirectXApi.h"
#include "Renderer/GeometryGenerator.h"
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification& pSpecification)
{
    assert(!s_Instance, "Application already exist!");
    s_Instance = this;
	m_Window = nullptr;

    // === Window ===
    m_Window = new Engine::WindowsWindow(Engine::WindowProps(pSpecification.Name, 1280, 700));
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	Engine::DirectXApi::Initialize();
	
	std::vector vertices = {
		Engine::Vertex{DirectX::XMFLOAT3{-.5f, .5f, 0}, DirectX::XMFLOAT4(1, 0, 0, 1)},
		Engine::Vertex{DirectX::XMFLOAT3{.5f, .5f, 0}, DirectX::XMFLOAT4(0, 1, 0, 1)},
		Engine::Vertex{DirectX::XMFLOAT3{-.5f, -.5f, 0}, DirectX::XMFLOAT4(0, 0, 1, 1)}
	};
	std::vector<uint16_t> indices = { 0, 1, 2 };
	m_Quad = std::make_unique<Engine::Mesh>(vertices, indices);
}

Application::~Application()
{
	Engine::DirectXApi::Shutdown();
	delete m_Window;
}

void Application::Run()
{
	while (m_IsRunning)
	{
		if (!m_IsMinimized)
		{
			Engine::DirectXApi::BeginFrame();
			
			m_Quad->Draw();
			
			Engine::DirectXApi::EndFrame();
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

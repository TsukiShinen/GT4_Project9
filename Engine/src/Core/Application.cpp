#include "Application.h"

#include "Timestep.h"
#include "Debug/Log.h"
#include "Events/KeyEvent.h"
#include "Renderer/DirectXApi.h"
#include "Renderer/Resource/DirectXResourceManager.h"
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
		Engine::VertexTex{DirectX::XMFLOAT3{-.5f, .5f, 0}, DirectX::XMFLOAT2(0, 0)},
		Engine::VertexTex{DirectX::XMFLOAT3{.5f, .5f, 0}, DirectX::XMFLOAT2(0, 1)},
		Engine::VertexTex{DirectX::XMFLOAT3{-.5f, -.5f, 0}, DirectX::XMFLOAT2(1, 0)},
	};
	std::vector<uint16_t> indices = { 0, 1, 2};
	m_Triangle1 = std::make_unique<Engine::DirectXMesh>(vertices, indices, Engine::DirectXContext::Get()->GetTextureShader());
	m_Triangle1->SetTexture(Engine::DirectXContext::Get()->GetResourceManager().LoadTexture(L"Textures\\stone.dds", "Stone"));

	
	std::vector vertices2 = {
		Engine::VertexColor{DirectX::XMFLOAT3{.5f, -.5f, 0}, DirectX::XMFLOAT4(1, 0, 0, 1)},
		Engine::VertexColor{DirectX::XMFLOAT3{-.5f, -.5f, 0}, DirectX::XMFLOAT4(0, 1, 0, 1)},
		Engine::VertexColor{DirectX::XMFLOAT3{.5f, .5f, 0}, DirectX::XMFLOAT4(0, 0, 1, 1)},
	};
	std::vector<uint16_t> indices2 = { 0, 1, 2 };
	m_Triangle2 = std::make_unique<Engine::DirectXMesh>(vertices2, indices2, Engine::DirectXContext::Get()->GetBaseShader());
	
	__int64 countsPerSec;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSec));
	m_SecondsPerCount = 1.0 / static_cast<double>(countsPerSec);
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
			__int64 time;
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
			Engine::Timestep deltaTime = (time - m_LastFrameTime) * m_SecondsPerCount;
			m_LastFrameTime = time;
			m_Fps = 1.f / deltaTime.GetSeconds();
			
			Engine::DirectXApi::BeginFrame();
			
			m_Triangle1->Draw();
			m_Triangle2->Draw();
			
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
	Engine::DirectXApi::Resize(pEvent.GetWidth(), pEvent.GetHeight());
	return true;
}

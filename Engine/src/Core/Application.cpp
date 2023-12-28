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

class Engine::DirectXCamera;

Application* Application::s_Instance = nullptr;

void CreateMeshFromFile(const char* file, std::unique_ptr<Engine::DirectXMesh>& mesh)
{
	std::vector<Engine::VertexLit> vertices;
	Engine::ObjLoader::LoadObj(file, &vertices);
	std::vector<uint16_t> indices;
	for (size_t i = 0; i < vertices.size(); i++)
	{
		indices.push_back(i);
	}
	mesh = std::make_unique<Engine::DirectXMesh>(vertices, indices);
}

Application::Application(const ApplicationSpecification& pSpecification)
{
    assert(!s_Instance, "Application already exist!");
    s_Instance = this;
	m_Window = nullptr;

    // === Window ===
    m_Window = new Engine::WindowsWindow(Engine::WindowProps(pSpecification.Name, 1280, 700));
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	Engine::DirectXApi::Initialize();
	
	// Shaders
	s_Instance->m_SimpleShader = std::make_unique<Engine::DirectXSimpleShader>(Engine::VertexColor::GetLayout(), L"Shaders\\Builtin.Color.hlsl");
	s_Instance->m_TextureShader = std::make_unique<Engine::DirectXTextureShader>(Engine::VertexTex::GetLayout(), L"Shaders\\Builtin.Texture.hlsl");
	s_Instance->m_LitShader = std::make_unique<Engine::DirectXLitShader>(Engine::VertexLit::GetLayout(), L"Shaders\\Builtin.Lit.hlsl");

	// Texture
	Engine::Texture* stone = Engine::DirectXContext::Get()->GetResourceManager().LoadTexture(L"Textures\\stone.dds", "Stone");
	Engine::Texture* bingus = Engine::DirectXContext::Get()->GetResourceManager().LoadTexture(L"Textures\\bingus.dds", "Bingus");

	// Materials
	s_Instance->m_SimpleMaterial = std::make_unique<Engine::DirectXSimpleMaterial>(s_Instance->m_SimpleShader.get());
	s_Instance->m_TextureMaterial = std::make_unique<Engine::DirectXTextureMaterial>(s_Instance->m_TextureShader.get(), stone);
	s_Instance->m_LitMaterial = std::make_unique<Engine::DirectXLitMaterial>(s_Instance->m_LitShader.get());
	s_Instance->m_BingusMaterial = std::make_unique<Engine::DirectXLitMaterial>(s_Instance->m_LitShader.get());
	s_Instance->m_BingusMaterial->SetTexture(bingus);
	s_Instance->m_StoneMaterial = std::make_unique<Engine::DirectXLitMaterial>(s_Instance->m_LitShader.get());
	s_Instance->m_StoneMaterial->SetTexture(stone);
	s_Instance->m_StoneMaterial2 = std::make_unique<Engine::DirectXLitMaterial>(s_Instance->m_LitShader.get(), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.5f);
	s_Instance->m_StoneMaterial2->SetTexture(stone);

	// Init mesh
	CreateMeshFromFile(".\\Objs\\bingus.obj", m_BingusMesh);
	CreateMeshFromFile(".\\Objs\\bunnyex.obj", m_BunnyMesh);

	std::vector vertices3{
		Engine::VertexTex{DirectX::XMFLOAT3{-.5f, .5f, 0}, DirectX::XMFLOAT2(0, 0)},
		Engine::VertexTex{DirectX::XMFLOAT3{.5f, .5f, 0}, DirectX::XMFLOAT2(0, 1)},
		Engine::VertexTex{DirectX::XMFLOAT3{-.5f, -.5f, 0}, DirectX::XMFLOAT2(1, 0)},
	};
	std::vector<uint16_t> indices3 = { 0, 1, 2 };
	m_Triangle1 = std::make_unique<Engine::DirectXMesh>(vertices3, indices3);


	std::vector vertices2 = {
		Engine::VertexColor{DirectX::XMFLOAT3{.5f, -.5f, 0}, DirectX::XMFLOAT4(1, 0, 0, 1)},
		Engine::VertexColor{DirectX::XMFLOAT3{-.5f, -.5f, 0}, DirectX::XMFLOAT4(0, 1, 0, 1)},
		Engine::VertexColor{DirectX::XMFLOAT3{.5f, .5f, 0}, DirectX::XMFLOAT4(0, 0, 1, 1)},
	};
	std::vector<uint16_t> indices2 = { 0, 1, 2 };
	m_Triangle2 = std::make_unique<Engine::DirectXMesh>(vertices2, indices2);

	// Init objects

	s_Instance->m_BingusObject = std::make_unique<Engine::Object>(DirectX::XMFLOAT3(0, -0.3f, 0), m_BingusMesh.get(), s_Instance->m_BingusMaterial.get());
	s_Instance->m_BingusObject->GetTransform()->SetScale(DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f));
	s_Instance->m_BunnyObject = std::make_unique<Engine::Object>(DirectX::XMFLOAT3(2, 0, 0), m_BunnyMesh.get(), s_Instance->m_StoneMaterial.get());
	s_Instance->m_BunnyObject2 = std::make_unique<Engine::Object>(DirectX::XMFLOAT3(-2, 0, 0), m_BunnyMesh.get(), s_Instance->m_StoneMaterial2.get());

	
	__int64 countsPerSec;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSec));
	m_SecondsPerCount = 1.0 / static_cast<double>(countsPerSec);

	__int64 time;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
	m_LastFrameTime = time;
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

			GameUpdate(deltaTime.GetSeconds());

			Engine::DirectXApi::BeginFrame();
			
			//m_Triangle1->Draw();
			//m_Triangle2->Draw();
			//m_Cube->Draw();
			m_BingusObject->GetTransform()->Rotate(deltaTime.GetSeconds(), 0, 0);
			m_BunnyObject->GetTransform()->Rotate(deltaTime.GetSeconds(), 0, 0);
			m_BunnyObject2->GetTransform()->Rotate(deltaTime.GetSeconds(), 0, 0);

			m_BingusObject->Render();
			m_BunnyObject->Render();
			m_BunnyObject2->Render();

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
	dispatcher.Dispatch<Engine::MouseMovedEvent>([](Engine::MouseMovedEvent& pEvent) {
		Engine::DirectXApi::CameraMouseEvent(pEvent.GetX(), pEvent.GetY());
		return true;
		});
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

void Application::GameUpdate(float dt)
{
	Engine::DirectXApi::UpdateCamera(dt);
}

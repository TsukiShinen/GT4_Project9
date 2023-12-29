#include "Sandbox.h"

#include "Core/ObjLoader.h"
#include "Renderer/DirectXApi.h"
#include "Renderer/Materials/DirectXLitMaterial.h"
#include "Renderer/Materials/DirectXSimpleMaterial.h"
#include "Renderer/Materials/DirectXTextureMaterial.h"
#include "Renderer/Resource/DirectXResourceManager.h"
#include "Renderer/Shaders/DirectXLitShader.h"
#include "Renderer/Shaders/DirectXSimpleShader.h"
#include "Renderer/Shaders/DirectXTextureShader.h"

Sandbox::Sandbox(const Engine::ApplicationSpecification& pSpecification)
	: Application(pSpecification)
{
	// Texture
	Engine::Texture* stone = Engine::DirectXContext::Get()->GetResourceManager().LoadTexture(L"Textures\\stone.dds", "Stone");
	Engine::Texture* bingus = Engine::DirectXContext::Get()->GetResourceManager().LoadTexture(L"Textures\\bingus.dds", "Bingus");
	Engine::Texture* white = Engine::DirectXContext::Get()->GetResourceManager().LoadTexture(L"Textures\\white.dds", "White");
	Engine::Texture* ground = Engine::DirectXContext::Get()->GetResourceManager().LoadTexture(L"Textures\\ground2.dds", "Ground");

	// Shaders
	m_SimpleShader = std::make_unique<Engine::DirectXSimpleShader>(Engine::VertexColor::GetLayout(), L"Shaders\\Builtin.Color.hlsl");
	m_TextureShader = std::make_unique<Engine::DirectXTextureShader>(Engine::VertexTex::GetLayout(), L"Shaders\\Builtin.Texture.hlsl");
	m_LitShader = std::make_unique<Engine::DirectXLitShader>(Engine::VertexLit::GetLayout(), L"Shaders\\Builtin.Lit.hlsl");

	// Materials
	m_SimpleMaterial = std::make_unique<Engine::DirectXSimpleMaterial>(m_SimpleShader.get());
	m_TextureMaterial = std::make_unique<Engine::DirectXTextureMaterial>(m_TextureShader.get(), stone);
	m_LitMaterial = std::make_unique<Engine::DirectXLitMaterial>(m_LitShader.get());
	m_BingusMaterial = std::make_unique<Engine::DirectXLitMaterial>(m_LitShader.get(), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.4f, 0.04f, bingus);
	m_StoneMaterial = std::make_unique<Engine::DirectXLitMaterial>(m_LitShader.get(), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.2f, 0.04f, stone);
	m_StoneMaterial2 = std::make_unique<Engine::DirectXLitMaterial>(m_LitShader.get(), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.8f, 0.04f, stone);
	m_GroundMaterial = std::make_unique<Engine::DirectXLitMaterial>(m_LitShader.get(), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.2f, 0.04f, ground, DirectX::XMFLOAT2(10, 10));

	for (size_t i = 0; i < 10; i++)
	{
		m_LitMaterials[i] = std::make_unique<Engine::DirectXLitMaterial>(m_LitShader.get(), DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f * i, 0.04f, white);
	}

	// Init mesh
	m_BingusMesh = Engine::DirectXMesh::CreateFromFile(".\\Objs\\bingus.obj");
	m_BunnyMesh = Engine::DirectXMesh::CreateFromFile(".\\Objs\\bunnyex.obj");
	m_FaceMesh = Engine::DirectXMesh::CreateFromFile(".\\Objs\\face.obj");
	m_SphereMesh = Engine::DirectXMesh::CreateFromFile(".\\Objs\\sphere.obj");

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
	m_BingusObject = std::make_unique<Engine::Object>(DirectX::XMFLOAT3(0, -0.3f, 0), m_BingusMesh.get(), m_BingusMaterial.get());
	m_BingusObject->GetTransform()->SetScale(DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f));
	m_BunnyObject = std::make_unique<Engine::Object>(DirectX::XMFLOAT3(2, 0, 0), m_BunnyMesh.get(), m_StoneMaterial.get());
	m_BunnyObject2 = std::make_unique<Engine::Object>(DirectX::XMFLOAT3(-2, 0, 0), m_BunnyMesh.get(), m_StoneMaterial2.get());
	m_Ground = std::make_unique<Engine::Object>(DirectX::XMFLOAT3(0, -0.4f, 0), m_FaceMesh.get(), m_GroundMaterial.get());
	m_Ground->GetTransform()->SetRotation(DirectX::XMFLOAT3(0, 0.0f, -DirectX::XMConvertToRadians(90)));
	m_Ground->GetTransform()->SetScale(DirectX::XMFLOAT3(10, 10, 10));

	for (size_t i = 0; i < 10; i++)
	{
		m_Spheres[i] = std::make_unique<Engine::Object>(DirectX::XMFLOAT3(i - 5.0f, 0, 4), m_SphereMesh.get(), m_LitMaterials[i].get());
		m_Spheres[i]->GetTransform()->SetScale(DirectX::XMFLOAT3(0.4f, 0.4f, 0.4f));
	}

	m_Timer = 0;
}

void Sandbox::Update(const Engine::Timestep pDeltaTime)
{
	Application::Update(pDeltaTime);

	m_Timer += pDeltaTime.GetSeconds();
	m_BingusObject->GetTransform()->SetRotation(DirectX::XMFLOAT3(std::sin(m_Timer * 5) / 2, 90, 0));
	m_BunnyObject->GetTransform()->Rotate(pDeltaTime.GetSeconds(), 0, 0);
	m_BunnyObject2->GetTransform()->Rotate(pDeltaTime.GetSeconds(), 0, 0);
}

void Sandbox::Draw()
{
	Application::Draw();
	
	m_BingusObject->Render();
	m_BunnyObject->Render();
	m_BunnyObject2->Render();
	m_Ground->Render();
	for (size_t i = 0; i < 10; i++)
	{
		m_Spheres[i]->Render();
	}
}

void Sandbox::OnEvent(Engine::Event& pEvent)
{
	Application::OnEvent(pEvent);
}

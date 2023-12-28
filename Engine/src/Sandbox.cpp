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
	// Shaders
		m_SimpleShader = std::make_unique<Engine::DirectXSimpleShader>(
			Engine::VertexColor::GetLayout(), L"Shaders\\Builtin.Color.hlsl");
		m_TextureShader = std::make_unique<Engine::DirectXTextureShader>(
			Engine::VertexTex::GetLayout(), L"Shaders\\Builtin.Texture.hlsl");
		m_LitShader = std::make_unique<Engine::DirectXLitShader>(
			Engine::VertexLit::GetLayout(), L"Shaders\\Builtin.Lit.hlsl");

		// Texture
		Engine::Texture* stone = Engine::DirectXContext::Get()->GetResourceManager().LoadTexture(
			L"Textures\\stone.dds", "Stone");
		Engine::Texture* bingus = Engine::DirectXContext::Get()->GetResourceManager().LoadTexture(
			L"Textures\\bingus.dds", "Bingus");

		// Materials
		m_SimpleMaterial = std::make_unique<Engine::DirectXSimpleMaterial>(m_SimpleShader.get());
		m_TextureMaterial = std::make_unique<Engine::DirectXTextureMaterial>(
			m_TextureShader.get(), stone);
		m_LitMaterial = std::make_unique<Engine::DirectXLitMaterial>(m_LitShader.get());
		m_BingusMaterial = std::make_unique<Engine::DirectXLitMaterial>(m_LitShader.get());
		m_BingusMaterial->SetTexture(bingus);

		// Init objects

		std::vector<Engine::VertexLit> vertices;
		Engine::ObjLoader::LoadObj(".\\Objs\\bingus.obj", &vertices);
		std::vector<uint16_t> indices;
		for (size_t i = 0; i < vertices.size(); i++)
		{
			indices.push_back(i);
		}
		m_Cube = std::make_unique<Engine::DirectXMesh>(vertices, indices,
		                                               (Engine::DirectXMaterial*)m_BingusMaterial.get());


		std::vector vertices3{
			Engine::VertexTex{DirectX::XMFLOAT3{-.5f, .5f, 0}, DirectX::XMFLOAT2(0, 0)},
			Engine::VertexTex{DirectX::XMFLOAT3{.5f, .5f, 0}, DirectX::XMFLOAT2(0, 1)},
			Engine::VertexTex{DirectX::XMFLOAT3{-.5f, -.5f, 0}, DirectX::XMFLOAT2(1, 0)},
		};
		std::vector<uint16_t> indices3 = {0, 1, 2};
		m_Triangle1 = std::make_unique<Engine::DirectXMesh>(vertices3, indices3,
		                                                    (Engine::DirectXMaterial*)m_TextureMaterial.get());


		std::vector vertices2 = {
			Engine::VertexColor{DirectX::XMFLOAT3{.5f, -.5f, 0}, DirectX::XMFLOAT4(1, 0, 0, 1)},
			Engine::VertexColor{DirectX::XMFLOAT3{-.5f, -.5f, 0}, DirectX::XMFLOAT4(0, 1, 0, 1)},
			Engine::VertexColor{DirectX::XMFLOAT3{.5f, .5f, 0}, DirectX::XMFLOAT4(0, 0, 1, 1)},
		};
		std::vector<uint16_t> indices2 = {0, 1, 2};
		m_Triangle2 = std::make_unique<Engine::DirectXMesh>(vertices2, indices2,
		                                                    (Engine::DirectXMaterial*)m_SimpleMaterial.get());
}

void Sandbox::Update(const Engine::Timestep pDeltaTime)
{
	Application::Update(pDeltaTime);
}

void Sandbox::Draw()
{
	Application::Draw();
	
	m_Triangle1->Draw();
	m_Triangle2->Draw();
	m_Cube->Draw();
}

void Sandbox::OnEvent(Engine::Event& pEvent)
{
	Application::OnEvent(pEvent);
}

#pragma once
#include "Core/Application.h"

class Sandbox : public Engine::Application
{
public:
	Sandbox(const Engine::ApplicationSpecification& pSpecification);

protected:
	void Update(Engine::Timestep pDeltaTime) override;
	void Draw() override;
	void OnEvent(Engine::Event& pEvent) override;

private:
	std::unique_ptr<Engine::DirectXSimpleShader> m_SimpleShader;
	std::unique_ptr<Engine::DirectXTextureShader> m_TextureShader;
	std::unique_ptr<Engine::DirectXLitShader> m_LitShader;

	std::unique_ptr<Engine::DirectXSimpleMaterial> m_SimpleMaterial;
	std::unique_ptr<Engine::DirectXTextureMaterial> m_TextureMaterial;
	std::unique_ptr<Engine::DirectXLitMaterial> m_LitMaterial;
	std::unique_ptr<Engine::DirectXLitMaterial> m_BingusMaterial;

	std::unique_ptr<Engine::DirectXMesh> m_Cube;
	std::unique_ptr<Engine::DirectXMesh> m_Triangle1;
	std::unique_ptr<Engine::DirectXMesh> m_Triangle2;
};
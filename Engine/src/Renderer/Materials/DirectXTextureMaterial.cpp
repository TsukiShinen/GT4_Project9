#include "DirectXTextureMaterial.h"

#include "Renderer/Shaders/DirectXShader.h"
#include "Renderer/DirectXContext.h"
#include "Renderer/Resource/DirectXResourceManager.h"

namespace Engine
{
	Engine::DirectXTextureMaterial::DirectXTextureMaterial(DirectXTextureShader* shader)
		: DirectXMaterial((DirectXShader*)shader)
	{
	}

	Engine::DirectXTextureMaterial::DirectXTextureMaterial(DirectXTextureShader* shader, Texture& texture)
		: DirectXMaterial((DirectXShader*)shader), m_Texture(texture)
	{
	}

	void Engine::DirectXTextureMaterial::Bind(DirectXMesh* mesh)
	{
		m_Shader->Bind(mesh);
		DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootDescriptorTable(0, DirectXContext::Get()->m_ResourceManager->GetTextureHandle(m_Texture));
	}
}
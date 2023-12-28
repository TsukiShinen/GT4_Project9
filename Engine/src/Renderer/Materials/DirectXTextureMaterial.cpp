#include "DirectXTextureMaterial.h"

#include "Renderer/Shaders/DirectXShader.h"
#include "Renderer/DirectXContext.h"
#include "Renderer/Resource/DirectXResourceManager.h"
#include "Renderer/DirectXCommandObject.h"

namespace Engine
{
	Engine::DirectXTextureMaterial::DirectXTextureMaterial(DirectXTextureShader* shader)
		: DirectXMaterial((DirectXShader*)shader)
	{
	}

	Engine::DirectXTextureMaterial::DirectXTextureMaterial(DirectXTextureShader* shader, Texture* texture)
		: DirectXMaterial((DirectXShader*)shader), m_Texture(texture)
	{
	}

	void Engine::DirectXTextureMaterial::Bind(const UploadBuffer<ObjectConstants>& objectConstantBuffer)
	{
		m_Shader->Bind(objectConstantBuffer);
		DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootDescriptorTable(0, DirectXContext::Get()->m_ResourceManager->GetTextureHandle(m_Texture));
	}
}

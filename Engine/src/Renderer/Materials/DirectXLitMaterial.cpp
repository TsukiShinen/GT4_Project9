#include "DirectXLitMaterial.h"

#include "Renderer/Shaders/DirectXShader.h"
#include "Renderer/DirectXContext.h"
#include "Renderer/Resource/DirectXResourceManager.h"
#include "Renderer/DirectXCommandObject.h"

namespace Engine
{
	DirectXLitMaterial::DirectXLitMaterial(DirectXLitShader* shader)
		: DirectXMaterial((DirectXShader*)shader), m_Data(), m_Texture(nullptr)
	{
		m_MatCB = std::make_unique<UploadBuffer<LitMaterialConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);
	}

	DirectXLitMaterial::DirectXLitMaterial(DirectXLitShader* shader, DirectX::XMFLOAT4 albedo,
	                                       DirectX::XMFLOAT4 specular, float smoothness, Texture* texture)
		: DirectXMaterial((DirectXShader*)shader), m_Data(albedo, specular, smoothness), m_Texture(texture)
	{
		m_MatCB = std::make_unique<UploadBuffer<LitMaterialConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);
	}

	void DirectXLitMaterial::SetTexture(Texture* texture)
	{
		m_Texture = texture;
	}

	void DirectXLitMaterial::Bind(DirectXMesh* mesh)
	{
		if (m_IsDirty)
		{
			m_MatCB->CopyData(0, m_Data);
			m_IsDirty = false;
		}

		m_Shader->Bind(mesh);

		DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootConstantBufferView(
			2, m_MatCB->Resource()->GetGPUVirtualAddress());
		if (m_Texture != nullptr)
			DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootDescriptorTable(
				3, DirectXContext::Get()->m_ResourceManager->GetTextureHandle(m_Texture));
	}
}

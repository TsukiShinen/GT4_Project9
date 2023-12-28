#include "DirectXLitMaterial.h"

#include "Renderer/Shaders/DirectXShader.h"
#include "Renderer/DirectXContext.h"
#include "Renderer/DirectXCommandObject.h"

namespace Engine
{
	DirectXLitMaterial::DirectXLitMaterial(DirectXLitShader* shader)
		: m_Data(), DirectXMaterial((DirectXShader*)shader)
	{
		m_MatCB = std::make_unique<UploadBuffer<LitMaterialConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);
	}

	DirectXLitMaterial::DirectXLitMaterial(DirectXLitShader* shader, DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness)
		: m_Data(diffuseAlbedo, fresnelR0, roughness), DirectXMaterial((DirectXShader*)shader)
	{
		m_MatCB = std::make_unique<UploadBuffer<LitMaterialConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);
	}

	void DirectXLitMaterial::Bind(DirectXMesh* mesh)
	{
		if (m_IsDirty)
		{
			m_MatCB->CopyData(0, m_Data);
			m_IsDirty = false;
		}

		m_Shader->Bind(mesh);

		DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootConstantBufferView(2, m_MatCB->Resource()->GetGPUVirtualAddress());
	}
}
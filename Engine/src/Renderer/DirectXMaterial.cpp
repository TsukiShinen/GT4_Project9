#include "DirectXMaterial.h"

#include "DirectXContext.h"
#include "DirectXCommandObject.h"

namespace Engine
{
	DirectXMaterial::DirectXMaterial()
		: m_Data(), m_IsDirty(true)
	{
		m_MatCB = std::make_unique<UploadBuffer<MaterialConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);
	}

	DirectXMaterial::DirectXMaterial(DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness)
		: m_Data(diffuseAlbedo, fresnelR0, roughness), m_IsDirty(true)
	{
		m_MatCB = std::make_unique<UploadBuffer<MaterialConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);
	}

	void DirectXMaterial::Bind()
	{
		if (m_IsDirty)
		{
			m_MatCB->CopyData(0, m_Data);
			m_IsDirty = false;
		}

		DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootConstantBufferView(2, m_MatCB->Resource()->GetGPUVirtualAddress());
	}
}
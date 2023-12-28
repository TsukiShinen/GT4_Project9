#pragma once
#include "DirectXFrameData.h"

namespace Engine
{
	struct MaterialConstants
	{
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.f, 1.f, 1.f, 1.f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;

		MaterialConstants()
		{
		}

		MaterialConstants(DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness)
			: DiffuseAlbedo(diffuseAlbedo), FresnelR0(fresnelR0), Roughness(roughness)
		{
		}
	};

	class DirectXMaterial
	{
	public:
		DirectXMaterial();
		DirectXMaterial(DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness);

		void Bind();

	private:

		bool m_IsDirty;
		MaterialConstants m_Data;
		std::unique_ptr<UploadBuffer<MaterialConstants>> m_MatCB = nullptr;
	};
}



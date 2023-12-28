#pragma once

#include "DirectXMaterial.h"
#include "Renderer/DirectXContext.h"

namespace Engine
{
	struct LitMaterialConstants
	{
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.f, 1.f, 1.f, 1.f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;

		LitMaterialConstants()
		{
		}

		LitMaterialConstants(DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness)
			: DiffuseAlbedo(diffuseAlbedo), FresnelR0(fresnelR0), Roughness(roughness)
		{
		}
	};

	class DirectXLitMaterial : public DirectXMaterial
	{
	public:
		DirectXLitMaterial(DirectXLitShader* shader);
		DirectXLitMaterial(DirectXLitShader* shader, DirectX::XMFLOAT4 diffuseAlbedo, DirectX::XMFLOAT3 fresnelR0, float roughness);

		void Bind(DirectXMesh* mesh) override;

	private:
		LitMaterialConstants m_Data;
		std::unique_ptr<UploadBuffer<LitMaterialConstants>> m_MatCB = nullptr;
	};
}
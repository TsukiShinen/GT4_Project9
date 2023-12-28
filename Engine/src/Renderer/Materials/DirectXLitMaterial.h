#pragma once

#include "DirectXMaterial.h"
#include "Renderer/Resource/Texture.h"
#include "Renderer/DirectXContext.h"
#include "Renderer/UploadBuffer.h"

namespace Engine
{
	struct LitMaterialConstants
	{
		alignas(16) DirectX::XMFLOAT4 Albedo = {1.f, 1.f, 1.f, 1.f};
		alignas(16) DirectX::XMFLOAT4 Specular = {0.1f, 0.1f, 0.1f, 1.f};
		alignas(16) float SpecularPower = 100.0f;

		LitMaterialConstants()
		{
		}

		LitMaterialConstants(DirectX::XMFLOAT4 albedo, DirectX::XMFLOAT4 specular, float specularPower)
			: Albedo(albedo), Specular(specular), SpecularPower(specularPower)
		{
		}
	};

	class DirectXLitMaterial : public DirectXMaterial
	{
	public:
		DirectXLitMaterial(DirectXLitShader* shader);
		DirectXLitMaterial(DirectXLitShader* shader, DirectX::XMFLOAT4 albedo, DirectX::XMFLOAT4 specular,
		                   float smoothness, Texture* texture = nullptr);

		void Bind(const UploadBuffer<ObjectConstants>& objectConstantBuffer) override;
		void SetTexture(Texture* texture);

	private:
		LitMaterialConstants m_Data;
		std::unique_ptr<UploadBuffer<LitMaterialConstants>> m_MatCB = nullptr;
		Texture* m_Texture;
	};
}

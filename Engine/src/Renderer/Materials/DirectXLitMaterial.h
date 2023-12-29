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
		alignas(4) float Smoothness = 0.5f;
		alignas(4) float Fresnel = 0.04f;

		LitMaterialConstants()
		{
		}

		LitMaterialConstants(DirectX::XMFLOAT4 albedo, DirectX::XMFLOAT4 specular, float smoothness, float fresnel = 0.04f)
			: Albedo(albedo), Specular(specular), Smoothness(smoothness), Fresnel(fresnel)
		{
		}
	};

	class DirectXLitMaterial : public DirectXMaterial
	{
	public:
		DirectXLitMaterial(DirectXLitShader* shader);
		DirectXLitMaterial(DirectXLitShader* shader, DirectX::XMFLOAT4 albedo, DirectX::XMFLOAT4 specular,
		                   float smoothness, float fresnel = 0.04f, Texture* texture = nullptr);

		void Bind(const UploadBuffer<ObjectConstants>& objectConstantBuffer) override;
		void SetTexture(Texture* texture);

	private:
		LitMaterialConstants m_Data;
		std::unique_ptr<UploadBuffer<LitMaterialConstants>> m_MatCB = nullptr;
		Texture* m_Texture;
	};
}

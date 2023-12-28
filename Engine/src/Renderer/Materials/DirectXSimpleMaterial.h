#pragma once

#include "DirectXMaterial.h"
#include "Renderer/DirectXContext.h"

namespace Engine
{
	class DirectXSimpleShader;

	class DirectXSimpleMaterial : public DirectXMaterial
	{
	public:
		DirectXSimpleMaterial(DirectXSimpleShader* shader);

		void Bind(const UploadBuffer<ObjectConstants>& objectConstantBuffer) override;
	};
}



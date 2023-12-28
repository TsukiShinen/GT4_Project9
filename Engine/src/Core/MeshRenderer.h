#pragma once

#include "Renderer/MathHelper.h"
#include "Renderer/UploadBuffer.h"

namespace Engine
{
	class DirectXMesh;
	class DirectXMaterial;

	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	};

	class MeshRenderer
	{
	public:
		MeshRenderer(DirectXMesh* mesh, DirectXMaterial* material);

		void Draw(const DirectX::XMFLOAT4X4& transformMatrix);

	private:
		DirectXMesh* m_Mesh;
		DirectXMaterial* m_Material;

		std::unique_ptr<UploadBuffer<ObjectConstants>> m_ConstantBuffer = nullptr;
	};
}

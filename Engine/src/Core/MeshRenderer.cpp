#include "MeshRenderer.h"

#include "Renderer/DirectXContext.h"
#include "Renderer/DirectXMesh.h"
#include "Renderer/Materials/DirectXMaterial.h"

namespace Engine
{
	MeshRenderer::MeshRenderer(DirectXMesh* mesh, DirectXMaterial* material = nullptr)
		: m_Mesh(mesh), m_Material(material)
	{
		m_ConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);
	}

	void MeshRenderer::Draw(const DirectX::XMFLOAT4X4& transformMatrix)
	{
		const DirectX::XMMATRIX world = XMLoadFloat4x4(&transformMatrix);
		ObjectConstants objConstants;
		XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
		m_ConstantBuffer->CopyData(0, objConstants);

		m_Material->Bind(*m_ConstantBuffer);

		m_Mesh->Draw();
	}
}
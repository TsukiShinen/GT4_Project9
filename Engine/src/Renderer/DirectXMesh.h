#pragma once
#include <DirectXMath.h>

#include "DirectXCamera.h"
#include "DirectXCommandObject.h"
#include "DirectXContext.h"
#include "DirectXFrameData.h"
#include "DirectXSwapchain.h"
#include "MathHelper.h"
#include "Resource/Texture.h"

namespace Engine
{
	class DirectXMaterial;

    class DirectXMesh
    {

    public:
        template <typename T, typename = std::enable_if_t<std::is_base_of_v<Vertex, T>>>
        DirectXMesh(std::vector<T>& pVertices, std::vector<uint16_t>& pIndices);

		void Draw();

    private:
		int m_NumFramesDirty = DirectXSwapchain::k_SwapChainBufferCount;

		D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferGpu;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferGpu;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferUploader;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferUploader;

		D3D12_VERTEX_BUFFER_VIEW m_VertexBuffer;
		D3D12_INDEX_BUFFER_VIEW m_IndexBuffer;
		UINT m_IndexCount = 0;
	};

	template <typename T, typename>
	DirectXMesh::DirectXMesh(std::vector<T>& pVertices, std::vector<uint16_t>& pIndices)
		: m_IndexCount(pIndices.size())
	{
		// ===== Data =====
		DirectXContext::Get()->m_CommandObject->GetCommandAllocator()->Reset();
		DirectXContext::Get()->m_CommandObject->
		                       ResetList(DirectXContext::Get()->m_CommandObject->GetCommandAllocator());

		const auto verticesByteSize = static_cast<UINT>(pVertices.size()) * sizeof(T);
		const auto indicesByteSize = static_cast<UINT>(pIndices.size()) * sizeof(uint16_t);

		m_VertexBufferGpu = DirectXContext::CreateDefaultBuffer(
			DirectXContext::Get()->m_Device.Get(),
			DirectXContext::Get()->m_CommandObject->GetCommandList().Get(), pVertices.data(), verticesByteSize,
			m_VertexBufferUploader);

		m_IndexBufferGpu = DirectXContext::CreateDefaultBuffer(
			DirectXContext::Get()->m_Device.Get(),
			DirectXContext::Get()->m_CommandObject->GetCommandList().Get(), pIndices.data(), indicesByteSize,
			m_IndexBufferUploader);

		DirectXContext::Get()->m_CommandObject->Execute();
		DirectXContext::Get()->m_CommandObject->Flush();

		m_VertexBuffer.BufferLocation = m_VertexBufferGpu->GetGPUVirtualAddress();
		m_VertexBuffer.StrideInBytes = sizeof(T);
		m_VertexBuffer.SizeInBytes = verticesByteSize;

		m_IndexBuffer.BufferLocation = m_IndexBufferGpu->GetGPUVirtualAddress();
		m_IndexBuffer.Format = DXGI_FORMAT_R16_UINT;
		m_IndexBuffer.SizeInBytes = indicesByteSize;
		
		/* Create a crash a end of application
		m_VertexBufferUploader->Release();
		m_IndexBufferUploader->Release();*/
	}
}

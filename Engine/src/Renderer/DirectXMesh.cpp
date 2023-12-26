#include "DirectXMesh.h"

#include "DirectXCommandObject.h"
#include "DirectXContext.h"
#include "DirectXShader.h"
#include "Debug/Log.h"

namespace Engine
{
    DirectXMesh::DirectXMesh(std::vector<Vertex>& pVertices, std::vector<uint16_t>& pIndices)
        : m_IndexCount(pIndices.size())
    {
        // ===== Constant Buffer =====
        m_ConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);

        // ===== Data =====
        DirectXContext::Get()->m_CommandObject->ResetList(DirectXContext::Get()->m_CommandObject->GetCommandAllocator());

        const auto verticesByteSize = static_cast<UINT>(pVertices.size()) * sizeof(Vertex);
        const auto indicesByteSize = static_cast<UINT>(pVertices.size()) * sizeof(Vertex);
        
        ThrowIfFailed(D3DCreateBlob(verticesByteSize, &m_VertexBufferCpu));
        CopyMemory(m_VertexBufferCpu->GetBufferPointer(), pVertices.data(), verticesByteSize);

        ThrowIfFailed(D3DCreateBlob(indicesByteSize, &m_IndexBufferCpu));
        CopyMemory(m_IndexBufferCpu->GetBufferPointer(), pIndices.data(), indicesByteSize);

        m_VertexBufferGpu = d3dUtil::CreateDefaultBuffer(
            DirectXContext::Get()->m_Device.Get(),
            DirectXContext::Get()->m_CommandObject->GetCommandList().Get(), pVertices.data(), verticesByteSize,
            m_VertexBufferUploader);

        m_IndexBufferGpu = d3dUtil::CreateDefaultBuffer(
            DirectXContext::Get()->m_Device.Get(),
            DirectXContext::Get()->m_CommandObject->GetCommandList().Get(), pIndices.data(), indicesByteSize,
            m_IndexBufferUploader);

        m_VertexBuffer.BufferLocation = m_VertexBufferGpu->GetGPUVirtualAddress();
        m_VertexBuffer.StrideInBytes = sizeof(Vertex);
        m_VertexBuffer.SizeInBytes = verticesByteSize;
        
        m_IndexBuffer.BufferLocation = m_IndexBufferGpu->GetGPUVirtualAddress();
        m_IndexBuffer.Format = DXGI_FORMAT_R16_UINT;
        m_IndexBuffer.SizeInBytes = indicesByteSize;

        DirectXContext::Get()->m_CommandObject->Execute();
    }

    DirectXMesh::~DirectXMesh()
    {
    }

    void DirectXMesh::Draw()
    {
        if(m_NumFramesDirty > 0)
        {
            const DirectX::XMMATRIX world = XMLoadFloat4x4(&m_TransformMatrix);

            ObjectConstants objConstants;
            XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

            m_ConstantBuffer->CopyData(0, objConstants);

            m_NumFramesDirty--;
        }

        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetVertexBuffers(0, 1, &m_VertexBuffer);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetIndexBuffer(&m_IndexBuffer);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveType);

        // Offset to the CBV in the descriptor heap for this object and for this frame resource.
        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer->Resource()->GetGPUVirtualAddress());
        
        DirectXContext::Get()->m_CommandObject->GetCommandList()->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
    }
}

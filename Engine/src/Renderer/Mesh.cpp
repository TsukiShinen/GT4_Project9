#include "Mesh.h"

#include "DirectXCommandObject.h"
#include "DirectXContext.h"
#include "Shader.h"
#include "Debug/Log.h"

namespace Engine
{
    Mesh::Mesh(std::vector<Vertex>& pVertices, std::vector<uint16_t>& pIndices)
        : m_IndexCount(pIndices.size())
    {
        // ===== Constant Buffer =====
        m_ConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);

        // ===== Data =====
        DirectXContext::Get()->m_CommandObject->ResetList(DirectXContext::Get()->m_CommandObject->GetCommandAllocator());

        const auto verticesByteSize = static_cast<UINT>(pVertices.size()) * sizeof(Vertex);
        const auto indicesByteSize = static_cast<UINT>(pVertices.size()) * sizeof(Vertex);
        
        ThrowIfFailed(D3DCreateBlob(verticesByteSize, &m_Geometry.VertexBufferCPU));
        CopyMemory(m_Geometry.VertexBufferCPU->GetBufferPointer(), pVertices.data(), verticesByteSize);

        ThrowIfFailed(D3DCreateBlob(indicesByteSize, &m_Geometry.IndexBufferCPU));
        CopyMemory(m_Geometry.IndexBufferCPU->GetBufferPointer(), pIndices.data(), indicesByteSize);

        m_Geometry.VertexBufferGPU = d3dUtil::CreateDefaultBuffer(DirectXContext::Get()->m_Device.Get(),
            DirectXContext::Get()->m_CommandObject->GetCommandList().Get(), pVertices.data(), verticesByteSize, m_Geometry.VertexBufferUploader);

        m_Geometry.IndexBufferGPU = d3dUtil::CreateDefaultBuffer(DirectXContext::Get()->m_Device.Get(),
            DirectXContext::Get()->m_CommandObject->GetCommandList().Get(), pIndices.data(), indicesByteSize, m_Geometry.IndexBufferUploader);

        m_Geometry.VertexByteStride = sizeof(Vertex);
        m_Geometry.VertexBufferByteSize = verticesByteSize;
        m_Geometry.IndexFormat = DXGI_FORMAT_R16_UINT;
        m_Geometry.IndexBufferByteSize = indicesByteSize;

        DirectXContext::Get()->m_CommandObject->Execute();
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::Draw()
    {
        if(m_NumFramesDirty > 0)
        {
            const DirectX::XMMATRIX world = XMLoadFloat4x4(&m_TransformMatrix);

            ObjectConstants objConstants;
            XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

            m_ConstantBuffer->CopyData(0, objConstants);

            m_NumFramesDirty--;
        }

        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetPipelineState(DirectXContext::Get()->m_BasePipeline->GetState().Get());
        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootSignature(DirectXContext::Get()->m_BasePipeline->GetSignature().Get());

        auto vertexBufferView = m_Geometry.VertexBufferView();
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
        auto indexBufferView = m_Geometry.IndexBufferView();
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetIndexBuffer(&indexBufferView);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveType);

        // Offset to the CBV in the descriptor heap for this object and for this frame resource.
        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer->Resource()->GetGPUVirtualAddress());
        
        DirectXContext::Get()->m_CommandObject->GetCommandList()->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
    }
}

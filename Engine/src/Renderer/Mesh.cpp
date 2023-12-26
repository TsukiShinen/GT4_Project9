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
        // ===== Constant Buffer Heaps =====
        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
        cbvHeapDesc.NumDescriptors = 1;
        cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.NodeMask = 0;
        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateDescriptorHeap(&cbvHeapDesc,
            IID_PPV_ARGS(&m_ConstantBufferHeap)));

        m_ConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);

        const auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_ConstantBufferHeap->GetCPUDescriptorHandleForHeapStart());
        const UINT objCbByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
        cbvDesc.BufferLocation = m_ConstantBuffer->Resource()->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = objCbByteSize;

        DirectXContext::Get()->m_Device->CreateConstantBufferView(&cbvDesc, handle);

        // ===== Data =====
        DirectXContext::Get()->m_CommandObject->ResetList(DirectXContext::Get()->m_CommandObject->GetCommandAllocator());

        auto verticesByteSize = static_cast<UINT>(pVertices.size()) * sizeof(Vertex);
        auto indicesByteSize = static_cast<UINT>(pVertices.size()) * sizeof(Vertex);
        
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

        ID3D12DescriptorHeap* descriptorHeaps[] = { m_ConstantBufferHeap.Get()};
        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetPipelineState(DirectXContext::Get()->m_BasePipeline->GetState().Get());
        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootSignature(DirectXContext::Get()->m_BasePipeline->GetSignature().Get());

        auto vertexBufferView = m_Geometry.VertexBufferView();
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
        auto indexBufferView = m_Geometry.IndexBufferView();
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetIndexBuffer(&indexBufferView);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveType);

        // Offset to the CBV in the descriptor heap for this object and for this frame resource.
        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootDescriptorTable(0, m_ConstantBufferHeap->GetGPUDescriptorHandleForHeapStart());
        
        DirectXContext::Get()->m_CommandObject->GetCommandList()->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
    }
}

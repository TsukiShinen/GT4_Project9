#include "DirectXMesh.h"

#include "DirectXCommandObject.h"
#include "DirectXContext.h"
#include "Shaders/DirectXShader.h"
#include "Debug/Log.h"

namespace Engine
{
    DirectXMesh::DirectXMesh(std::vector<VertexTex>& pVertices, std::vector<uint16_t>& pIndices, std::shared_ptr<DirectXShader> pShader)
        : m_IndexCount(pIndices.size()), m_Shader(pShader)
    {
        // ===== Constant Buffer =====
        m_ConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);

        // ===== Data =====
        DirectXContext::Get()->m_CommandObject->GetCommandAllocator()->Reset();
        DirectXContext::Get()->m_CommandObject->ResetList(DirectXContext::Get()->m_CommandObject->GetCommandAllocator());

        const auto verticesByteSize = static_cast<UINT>(pVertices.size()) * sizeof(VertexTex);
        const auto indicesByteSize = static_cast<UINT>(pVertices.size()) * sizeof(VertexTex);
        
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
        m_VertexBuffer.StrideInBytes = sizeof(VertexTex);
        m_VertexBuffer.SizeInBytes = verticesByteSize;
        
        m_IndexBuffer.BufferLocation = m_IndexBufferGpu->GetGPUVirtualAddress();
        m_IndexBuffer.Format = DXGI_FORMAT_R16_UINT;
        m_IndexBuffer.SizeInBytes = indicesByteSize;

        DirectXContext::Get()->m_CommandObject->Execute();
        DirectXContext::Get()->m_CommandObject->Flush();
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

        m_Shader->Bind(this);
        
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetVertexBuffers(0, 1, &m_VertexBuffer);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetIndexBuffer(&m_IndexBuffer);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveType);
        
        DirectXContext::Get()->m_CommandObject->GetCommandList()->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
    }
}

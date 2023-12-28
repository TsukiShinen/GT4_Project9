#pragma once
#include <DirectXMath.h>

#include "DirectXCommandObject.h"
#include "DirectXContext.h"
#include "DirectXFrameData.h"
#include "MathHelper.h"
#include "Resource/Texture.h"

namespace Engine
{
    class DirectXMaterial;

    struct ObjectConstants
    {
        DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
    };
    
    class DirectXMesh
    {
    public:
        template <typename T, typename = std::enable_if_t<std::is_base_of_v<Vertex, T>>>
        DirectXMesh(std::vector<T>& pVertices, std::vector<uint16_t>& pIndices, DirectXMaterial* pMaterial);

        void Draw();

        void SetTransformMatrix(DirectX::XMFLOAT4X4 transformMatrix);

        [[nodiscard]] UploadBuffer<ObjectConstants>& GetConstantBuffer() const { return *m_ConstantBuffer; }
        [[nodiscard]] Texture* GetTexture() { return m_Texture; }
        void SetTexture(Texture* pTexture) { m_Texture = pTexture; }

    private:
        DirectX::XMFLOAT4X4 m_TransformMatrix = MathHelper::Identity4x4();
        
        int m_NumFramesDirty = gNumFrameResources;
        
        D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        
        Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferGpu;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferGpu;

        Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferUploader;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferUploader;
        
        D3D12_VERTEX_BUFFER_VIEW m_VertexBuffer;
        D3D12_INDEX_BUFFER_VIEW m_IndexBuffer;
        UINT m_IndexCount = 0;

        DirectXMaterial* m_Material;
        
        std::unique_ptr<UploadBuffer<ObjectConstants>> m_ConstantBuffer = nullptr;

        Texture* m_Texture;
    };

    template <typename T, typename>
    DirectXMesh::DirectXMesh(std::vector<T>& pVertices, std::vector<uint16_t>& pIndices, DirectXMaterial* pMaterial)
            : m_IndexCount(pIndices.size()), m_Material(pMaterial)
    {
        // ===== Constant Buffer =====
        m_ConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(DirectXContext::Get()->m_Device.Get(), 1, true);

        // ===== Data =====
        DirectXContext::Get()->m_CommandObject->GetCommandAllocator()->Reset();
        DirectXContext::Get()->m_CommandObject->ResetList(DirectXContext::Get()->m_CommandObject->GetCommandAllocator());

        const auto verticesByteSize = static_cast<UINT>(pVertices.size()) * sizeof(T);
        const auto indicesByteSize = static_cast<UINT>(pIndices.size()) * sizeof(uint16_t);

        m_VertexBufferGpu = d3dUtil::CreateDefaultBuffer(
            DirectXContext::Get()->m_Device.Get(),
            DirectXContext::Get()->m_CommandObject->GetCommandList().Get(), pVertices.data(), verticesByteSize,
            m_VertexBufferUploader);

        m_IndexBufferGpu = d3dUtil::CreateDefaultBuffer(
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
        
        m_VertexBufferUploader->Release();
        m_IndexBufferUploader->Release();
    }
}

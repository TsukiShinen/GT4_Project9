#pragma once
#include <DirectXMath.h>

#include "d3dUtil.h"
#include "DirectXFrameData.h"
#include "MathHelper.h"

namespace Engine
{
    struct ObjectConstants
    {
        DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
    };
    
    class Mesh
    {
    public:
        Mesh(std::vector<Vertex>& pVertices, std::vector<uint16_t>& pIndices);
        ~Mesh();
        void Draw();

    private:
        DirectX::XMFLOAT4X4 m_TransformMatrix = MathHelper::Identity4x4();
        
        int m_NumFramesDirty = gNumFrameResources;
        
        D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        Microsoft::WRL::ComPtr<ID3DBlob> m_VertexBufferCpu;
        Microsoft::WRL::ComPtr<ID3DBlob> m_IndexBufferCpu;
        
        Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferGpu;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferGpu;

        Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferUploader;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferUploader;
        
        D3D12_VERTEX_BUFFER_VIEW m_VertexBuffer;
        D3D12_INDEX_BUFFER_VIEW m_IndexBuffer;
        UINT m_IndexCount = 0;
        
        std::unique_ptr<UploadBuffer<ObjectConstants>> m_ConstantBuffer = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ConstantBufferHeap = nullptr;
    };
    
}

#pragma once
#include <DirectXMath.h>

#include "DirectXFrameData.h"
#include "MathHelper.h"
#include "Resource/Texture.h"

namespace Engine
{
    class DirectXShader;

    struct ObjectConstants
    {
        DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
    };
    
    class DirectXMesh
    {
    public:
        DirectXMesh(std::vector<VertexTex>& pVertices, std::vector<uint16_t>& pIndices, std::shared_ptr<DirectXShader> pShader);

        void Draw();

        [[nodiscard]] UploadBuffer<ObjectConstants>& GetConstantBuffer() const { return *m_ConstantBuffer; }
        [[nodiscard]] Texture& GetTexture() { return m_Texture; }
        void SetTexture(const Texture& pTexture) { m_Texture = pTexture; }

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

        std::shared_ptr<DirectXShader> m_Shader;
        
        std::unique_ptr<UploadBuffer<ObjectConstants>> m_ConstantBuffer = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ConstantBufferHeap = nullptr;

        Texture m_Texture;
    };
    
}

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
    
    class Model
    {
    public:
        Model(std::vector<Vertex>& pVertices, std::vector<uint16_t>& pIndices);
        ~Model();
        void Draw();

    private:
        DirectX::XMFLOAT4X4 m_TransformMatrix = MathHelper::Identity4x4();
        
        int m_NumFramesDirty = gNumFrameResources;
        
        D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        
        MeshGeometry m_Geometry;
        UINT m_IndexCount = 0;
        
        std::unique_ptr<UploadBuffer<ObjectConstants>> m_ConstantBuffer = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ConstantBufferHeap = nullptr;
    };
    
}

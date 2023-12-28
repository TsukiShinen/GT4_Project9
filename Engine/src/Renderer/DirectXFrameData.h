#pragma once
#include <cstdint>

#include "UploadBuffer.h"

namespace Engine
{

    struct PassConstants
    {
        DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
    };
    
    struct Vertex {};

    struct VertexColor : Vertex
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT4 Color{1, 1, 1, 1};

        VertexColor(const DirectX::XMFLOAT3 pPosition)
            : Vertex(), Position(pPosition) {}
        VertexColor(const DirectX::XMFLOAT3 pPosition, const DirectX::XMFLOAT4 pColor)
            : Vertex(), Position(pPosition), Color(pColor) {}

        static std::vector<D3D12_INPUT_ELEMENT_DESC> GetLayout()
        {
            return {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
            };
        }
    };

    struct VertexTex : Vertex
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT2 TexCoord;
        
        VertexTex(const DirectX::XMFLOAT3 pPosition, const DirectX::XMFLOAT2 pTexCoord)
            : Vertex(), Position(pPosition), TexCoord(pTexCoord) {}

        static std::vector<D3D12_INPUT_ELEMENT_DESC> GetLayout()
        {
            return {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
            };
        }
    };
    
    struct DirectXFrameData
    {
        DirectXFrameData(ID3D12Device* pDevice, UINT pPassCount)
        {
            ThrowIfFailed(pDevice->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

            PassCB = std::make_unique<UploadBuffer<PassConstants>>(pDevice, pPassCount, true);
        }
        DirectXFrameData(const DirectXFrameData& pFrameData) = delete;
        DirectXFrameData& operator=(const DirectXFrameData& pFrameData) = delete;
        ~DirectXFrameData() = default;

        // We cannot reset the allocator until the GPU is done processing the commands.
        // So each frame needs their own allocator.
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

        // We cannot update a cbuffer until the GPU is done processing the commands
        // that reference it.  So each frame needs their own cbuffers.
        std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;

        // Fence value to mark commands up to this fence point.  This lets us
        // check if these frame resources are still in use by the GPU.
        UINT64 Fence = 0;
    };
    
}

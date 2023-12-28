#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "Renderer/DirectXMesh.h"

namespace Engine
{
    class DirectXShader
    {
    public:
        Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetSignature() { return m_RootSignature; }
        Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetState() { return m_PipelineState; }

        virtual void Bind(DirectXMesh* pMesh) = 0;

    protected:
        
        static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
        
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
    };
    
}

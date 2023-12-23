#pragma once

#include "DirectXContext.h"

namespace Engine
{
    class DirectXPipeline
    {
    public:
        DirectXPipeline(const std::vector<D3D12_INPUT_ELEMENT_DESC>& pLayout, const std::wstring& pShaderPath);
        ~DirectXPipeline();

        Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetSignature() { return m_RootSignature; }
        Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetState() { return m_PipelineState; }
    private:
        void InitializeSignature();
        void InitializePipelineState(std::vector<D3D12_INPUT_ELEMENT_DESC> pLayout,
                                     Microsoft::WRL::ComPtr<ID3DBlob> pVsByteCode,
                                     Microsoft::WRL::ComPtr<ID3DBlob> pPsByteCode);
        
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
    };
    
}

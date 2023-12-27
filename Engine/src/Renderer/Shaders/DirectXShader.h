#pragma once

#include "../DirectXContext.h"

namespace Engine
{
    class DirectXShader
    {
    public:
        DirectXShader(const std::vector<D3D12_INPUT_ELEMENT_DESC>& pLayout, const std::wstring& pShaderPath);
        ~DirectXShader();

        Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetSignature() { return m_RootSignature; }
        Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetState() { return m_PipelineState; }

        void Begin();

    private:
        void InitializeSignature();
        void InitializePipelineState(std::vector<D3D12_INPUT_ELEMENT_DESC> pLayout,
                                     Microsoft::WRL::ComPtr<ID3DBlob> pVsByteCode,
                                     Microsoft::WRL::ComPtr<ID3DBlob> pPsByteCode);
        
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
    };
    
}

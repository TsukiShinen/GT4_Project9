#pragma once

#include "DirectXShader.h"
#include "../DirectXContext.h"

namespace Engine
{
    class DirectXTextureShader : public DirectXShader
    {
    public:
        DirectXTextureShader(const std::vector<D3D12_INPUT_ELEMENT_DESC>& pLayout, const std::wstring& pShaderPath);

        void Bind(DirectXMesh* pMesh) override;

    private:
        void InitializeSignature();
        void InitializePipelineState(std::vector<D3D12_INPUT_ELEMENT_DESC> pLayout,
                                     Microsoft::WRL::ComPtr<ID3DBlob> pVsByteCode,
                                     Microsoft::WRL::ComPtr<ID3DBlob> pPsByteCode);


        static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
    };
    
}

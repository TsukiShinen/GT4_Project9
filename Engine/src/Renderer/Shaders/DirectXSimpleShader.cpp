#include "DirectXSimpleShader.h"

#include <comdef.h>

#include "../DirectXSwapchain.h"
#include "../DirectXCommandObject.h"
#include "Debug/Log.h"

namespace Engine
{

    DirectXSimpleShader::DirectXSimpleShader(const std::vector<D3D12_INPUT_ELEMENT_DESC>& pLayout, const std::wstring& pShaderPath)
    {
        const Microsoft::WRL::ComPtr<ID3DBlob> vsByteCode = DirectXContext::CompileShader(pShaderPath, nullptr, "VS", "vs_5_0");
        const Microsoft::WRL::ComPtr<ID3DBlob> psByteCode = DirectXContext::CompileShader(pShaderPath, nullptr, "PS", "ps_5_0");

        InitializeSignature();

        InitializePipelineState(pLayout, vsByteCode, psByteCode);
    }

    void DirectXSimpleShader::Bind(DirectXMesh* pMesh)
    {
        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetPipelineState(GetState().Get());
        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootSignature(GetSignature().Get());
        
        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootConstantBufferView(1, DirectXContext::Get()->CurrentFrameData().PassCB->Resource()->GetGPUVirtualAddress());

        DirectXContext::Get()->m_CommandObject->GetCommandList()->SetGraphicsRootConstantBufferView(0, pMesh->GetConstantBuffer().Resource()->GetGPUVirtualAddress());
    }

    void DirectXSimpleShader::InitializeSignature()
    {
        CD3DX12_ROOT_PARAMETER slotRootParameter[3];

        slotRootParameter[0].InitAsConstantBufferView(0);
        slotRootParameter[1].InitAsConstantBufferView(1);
        slotRootParameter[2].InitAsConstantBufferView(2);

        const CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, 0, nullptr,
                                                      D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
        const HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                                       serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

        if (errorBlob != nullptr)
        {
            CORE_ERROR(static_cast<char*>(errorBlob->GetBufferPointer()));
        }
        THROW_IF_FAILED(hr);

        THROW_IF_FAILED(DirectXContext::Get()->m_Device->CreateRootSignature(
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(&m_RootSignature)));
    }
    
    void DirectXSimpleShader::InitializePipelineState(std::vector<D3D12_INPUT_ELEMENT_DESC> pLayout, Microsoft::WRL::ComPtr<ID3DBlob> pVsByteCode, Microsoft::WRL::ComPtr<ID3DBlob> pPsByteCode)
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
        ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
        psoDesc.InputLayout = {pLayout.data(), static_cast<UINT>(pLayout.size())};
        psoDesc.pRootSignature = m_RootSignature.Get();
        psoDesc.VS =
        {
            static_cast<BYTE*>(pVsByteCode->GetBufferPointer()),
            pVsByteCode->GetBufferSize()
        };
        psoDesc.PS =
        {
            static_cast<BYTE*>(pPsByteCode->GetBufferPointer()),
            pPsByteCode->GetBufferSize()
        };
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DirectXSwapchain::k_BackBufferFormat;
        psoDesc.SampleDesc.Count = DirectXContext::Get()->m_4xMsaaState ? 4 : 1;
        psoDesc.SampleDesc.Quality = DirectXContext::Get()->m_4xMsaaState
                                         ? (DirectXContext::Get()->m_4xMsaaQuality - 1)
                                         : 0;
        psoDesc.DSVFormat = DirectXSwapchain::k_DepthStencilFormat;
        THROW_IF_FAILED(DirectXContext::Get()->m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
    }
}

#include "DirectXApi.h"

#include <iostream>

#include "DirectXCommandObject.h"
#include "DirectXCamera.h"
#include "DirectXSwapchain.h"
#include "Core/Application.h"
#include "Resource/DirectXResourceManager.h"

namespace Engine
{
    void DirectXApi::Initialize()
    {
        InitializeDebug();
	    DirectXContext::Initialize();
#ifdef _DEBUG
        LogAdapters();
#endif
    }

    void DirectXApi::Shutdown()
    {
        DirectXContext::Shutdown();
    }

    void DirectXApi::Resize(const int pWidth, const int pHeight)
    {
        DirectXContext::Get()->m_Swapchain->Resize(pWidth, pHeight);
        DirectXContext::Get()->m_Camera->Resize(pWidth, pHeight);
    }

    void DirectXApi::BeginFrame()
    {
        auto& cmdListAlloc = DirectXContext::Get()->CurrentFrameData().CmdListAlloc;
        ThrowIfFailed(cmdListAlloc->Reset());
        ThrowIfFailed(DirectXContext::Get()->m_CommandObject->ResetList(cmdListAlloc));

        DirectXContext::Get()->m_CommandObject->GetCommandList()->RSSetViewports(1, &DirectXContext::Get()->m_Swapchain->GetViewport());
        DirectXContext::Get()->m_CommandObject->GetCommandList()->RSSetScissorRects(1, &DirectXContext::Get()->m_Swapchain->GetScissorRect());

        // Indicate a state transition on the resource usage.
        const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(DirectXContext::Get()->m_Swapchain->GetCurrentBackBuffer(),
                                                                  D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->ResourceBarrier(1, &barrier);

        // Clear the back buffer and depth buffer.
        DirectXContext::Get()->m_CommandObject->GetCommandList()->ClearRenderTargetView(DirectXContext::Get()->m_Swapchain->GetCurrentBackBufferView(), DirectX::Colors::Gray, 0, nullptr);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->ClearDepthStencilView(DirectXContext::Get()->m_Swapchain->GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        DirectXContext::Get()->m_ResourceManager->BindDescriptorsHeap();
        
        const auto renderTargetDescriptor = DirectXContext::Get()->m_Swapchain->GetCurrentBackBufferView();
        const auto depthStencilDescriptor = DirectXContext::Get()->m_Swapchain->GetDepthStencilView();
        DirectXContext::Get()->m_CommandObject->GetCommandList()->OMSetRenderTargets(1,
            &renderTargetDescriptor, true,
            &depthStencilDescriptor);

        DirectXContext::Get()->m_Camera->Update();

        const auto currPassCb = DirectXContext::Get()->CurrentFrameData().PassCB.get();
        currPassCb->CopyData(0, DirectXContext::Get()->m_Camera->m_MainPassCB);
    }

    void DirectXApi::EndFrame()
    {
        const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(DirectXContext::Get()->m_Swapchain->GetCurrentBackBuffer(),
                                    D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        DirectXContext::Get()->m_CommandObject->GetCommandList()->ResourceBarrier(1, &barrier);
        
        DirectXContext::Get()->m_CommandObject->Execute();
        DirectXContext::Get()->m_Swapchain->Present();

        DirectXContext::Get()->CurrentFrameData().Fence = ++DirectXContext::Get()->m_CommandObject->GetCurrentFenceIndex();
        DirectXContext::Get()->m_CommandObject->GetCommandQueue()->Signal(
            DirectXContext::Get()->m_CommandObject->GetFence().Get(),
            DirectXContext::Get()->m_CommandObject->GetCurrentFenceIndex());
    }

    void DirectXApi::UpdateCamera(float dt)
    {
        DirectXContext::Get()->m_Camera->GameUpdate(dt);
    }

    void DirectXApi::InitializeDebug()
    {
#if defined(DEBUG) || defined(_DEBUG)
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
#endif
    }

    void DirectXApi::LogAdapters()
    {
        UINT i = 0;
        IDXGIAdapter* adapter = nullptr;
        std::vector<IDXGIAdapter*> adapterList;
        while(DirectXContext::Get()->m_Factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC desc;
            adapter->GetDesc(&desc);

            std::wstring text = L"***Adapter: ";
            text += desc.Description;
            text += L"\n";

            OutputDebugString(text.c_str());

            adapterList.push_back(adapter);
        
            ++i;
        }

        for(size_t i = 0; i < adapterList.size(); ++i)
        {
            LogAdapterOutputs(adapterList[i]);
            ReleaseCom(adapterList[i]);
        }
    }

    void DirectXApi::LogAdapterOutputs(IDXGIAdapter* pAdapter)
    {
        UINT i = 0;
        IDXGIOutput* output = nullptr;
        while(pAdapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_OUTPUT_DESC desc;
            output->GetDesc(&desc);
        
            std::wstring text = L"***Output: ";
            text += desc.DeviceName;
            text += L"\n";
            OutputDebugString(text.c_str());

            LogOutputDisplayModes(output, DirectXSwapchain::k_BackBufferFormat);

            ReleaseCom(output);

            ++i;
        }
    }

    void DirectXApi::LogOutputDisplayModes(IDXGIOutput* pOutput, const DXGI_FORMAT pFormat)
    {
        UINT count = 0;
        const UINT flags = 0;

        // Call with nullptr to get list count.
        pOutput->GetDisplayModeList(pFormat, flags, &count, nullptr);

        std::vector<DXGI_MODE_DESC> modeList(count);
        pOutput->GetDisplayModeList(pFormat, flags, &count, &modeList[0]);

        for(auto& x : modeList)
        {
            UINT n = x.RefreshRate.Numerator;
            UINT d = x.RefreshRate.Denominator;
            std::wstring text =
                L"Width = " + std::to_wstring(x.Width) + L" " +
                L"Height = " + std::to_wstring(x.Height) + L" " +
                L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
                L"\n";

            ::OutputDebugString(text.c_str());
        }
    }
}

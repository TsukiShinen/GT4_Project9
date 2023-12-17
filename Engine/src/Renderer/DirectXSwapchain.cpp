#include "DirectXSwapchain.h"

#include "DirectXCommandObjects.h"
#include "Core/Application.h"

namespace Engine
{
    DirectXSwapchain::DirectXSwapchain(const uint32_t pWidth, const uint32_t pHeight)
    {
    	InitializeMsaa();
    	
        m_Swapchain.Reset();

        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width = pWidth;
        sd.BufferDesc.Height = pHeight;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = k_BackBufferFormat;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
        sd.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = k_SwapChainBufferCount;
        sd.OutputWindow = Application::Get()->GetWindow()->GetWindow();
        sd.Windowed = true;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        // Note: Swap chain uses queue to perform flush.
        ThrowIfFailed(DirectXContext::Get()->m_Factory->CreateSwapChain(
            DirectXContext::Get()->m_CommandObjects->m_CommandQueue.Get(),
            &sd, 
            m_Swapchain.GetAddressOf()));
    }

    DirectXSwapchain::~DirectXSwapchain()
    {
    }

    ID3D12Resource* DirectXSwapchain::CurrentBackBuffer() const
    {
		return m_SwapchainBuffers[m_CurrentBackBuffer].Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DirectXSwapchain::CurrentBackBufferView() const
    {
    	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			DirectXContext::Get()->m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_CurrentBackBuffer,
			DirectXContext::Get()->m_RtvDescriptorSize);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DirectXSwapchain::DepthStencilView() const
    {
		return DirectXContext::Get()->m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
    }

    void DirectXSwapchain::Resize(uint32_t pWidth, uint32_t pHeight)
    {
    	// Flush before changing any resources.
		DirectXContext::Get()->m_CommandObjects->Flush();

	    ThrowIfFailed(DirectXContext::Get()->m_CommandObjects->m_CommandList->Reset(DirectXContext::Get()->m_CommandObjects->m_DirectCmdListAlloc.Get(), nullptr));

		// Release the previous resources we will be recreating.
		for (int i = 0; i < k_SwapChainBufferCount; ++i)
			m_SwapchainBuffers[i].Reset();
	    m_DepthStencilBuffer.Reset();
		
		// Resize the swap chain.
	    ThrowIfFailed(m_Swapchain->ResizeBuffers(
			k_SwapChainBufferCount, 
			pWidth, pHeight, 
			k_BackBufferFormat, 
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		m_CurrentBackBuffer = 0;
	 
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(DirectXContext::Get()->m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < k_SwapChainBufferCount; i++)
		{
			ThrowIfFailed(m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&m_SwapchainBuffers[i])));
			DirectXContext::Get()->m_Device->CreateRenderTargetView(m_SwapchainBuffers[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, DirectXContext::Get()->m_RtvDescriptorSize);
		}

	    // Create the depth/stencil buffer and view.
	    D3D12_RESOURCE_DESC depthStencilDesc;
	    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	    depthStencilDesc.Alignment = 0;
	    depthStencilDesc.Width = pWidth;
	    depthStencilDesc.Height = pHeight;
	    depthStencilDesc.DepthOrArraySize = 1;
	    depthStencilDesc.MipLevels = 1;

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	    depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	    depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	    D3D12_CLEAR_VALUE optClear;
	    optClear.Format = k_DepthStencilFormat;
	    optClear.DepthStencil.Depth = 1.0f;
	    optClear.DepthStencil.Stencil = 0;
    	auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	    ThrowIfFailed(DirectXContext::Get()->m_Device->CreateCommittedResource(
	        &properties,
			D3D12_HEAP_FLAG_NONE,
	        &depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
	        &optClear,
	        IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));

	    // Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = k_DepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
	    DirectXContext::Get()->m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	    // Transition the resource from its initial state to be used as a depth buffer.
    	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		DirectXContext::Get()->m_CommandObjects->m_CommandList->ResourceBarrier(1, &barrier);
		
	    // Execute the resize commands.
	    ThrowIfFailed(DirectXContext::Get()->m_CommandObjects->m_CommandList->Close());
	    ID3D12CommandList* cmdsLists[] = { DirectXContext::Get()->m_CommandObjects->m_CommandList.Get() };
	    DirectXContext::Get()->m_CommandObjects->m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until resize is complete.
		DirectXContext::Get()->m_CommandObjects->Flush();

		// Update the viewport transform to cover the client area.
		m_ScreenViewport.TopLeftX = 0;
		m_ScreenViewport.TopLeftY = 0;
		m_ScreenViewport.Width    = static_cast<float>(pWidth);
		m_ScreenViewport.Height   = static_cast<float>(pHeight);
		m_ScreenViewport.MinDepth = 0.0f;
		m_ScreenViewport.MaxDepth = 1.0f;

	    m_ScissorRect = { 0, 0, static_cast<int>(pWidth), static_cast<int>(pHeight) };
    }

    void DirectXSwapchain::Present()
    {
    	ThrowIfFailed(m_Swapchain->Present(0, 0));
    	m_CurrentBackBuffer = (m_CurrentBackBuffer + 1) % k_SwapChainBufferCount;

    	// Wait until frame commands are complete.  This waiting is inefficient and is
    	// done for simplicity.  Later we will show how to organize our rendering code
    	// so we do not have to wait per frame.
    	DirectXContext::Get()->m_CommandObjects->Flush();
    }

    void DirectXSwapchain::InitializeMsaa()
    {
    	// Check 4X MSAA quality support for our back buffer format.
    	// All Direct3D 11 capable devices support 4X MSAA for all render 
    	// target formats, so we only need to check quality support.

    	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    	msQualityLevels.Format = k_BackBufferFormat;
    	msQualityLevels.SampleCount = 4;
    	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    	msQualityLevels.NumQualityLevels = 0;
    	ThrowIfFailed(DirectXContext::Get()->m_Device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)));

    	m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
    	assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
    }
}

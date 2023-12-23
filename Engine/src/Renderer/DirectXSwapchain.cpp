#include "DirectXSwapchain.h"

#include "DirectXCommandObject.h"
#include "Core/Application.h"

namespace Engine
{
    DirectXSwapchain::DirectXSwapchain(const uint32_t pWidth, const uint32_t pHeight)
    {
        m_Swapchain.Reset();

        DXGI_SWAP_CHAIN_DESC swapchainInfo;
        swapchainInfo.BufferDesc.Width = pWidth;
        swapchainInfo.BufferDesc.Height = pHeight;
        swapchainInfo.BufferDesc.RefreshRate.Numerator = 60;
        swapchainInfo.BufferDesc.RefreshRate.Denominator = 1;
        swapchainInfo.BufferDesc.Format = k_BackBufferFormat;
        swapchainInfo.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapchainInfo.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapchainInfo.SampleDesc.Count = DirectXContext::Get()->m_4xMsaaState ? 4 : 1;
        swapchainInfo.SampleDesc.Quality = DirectXContext::Get()->m_4xMsaaState ? (DirectXContext::Get()->m_4xMsaaQuality - 1) : 0;
        swapchainInfo.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchainInfo.BufferCount = k_SwapChainBufferCount;
        swapchainInfo.OutputWindow = Application::Get()->GetWindow()->GetWindow();
        swapchainInfo.Windowed = true;
        swapchainInfo.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchainInfo.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        ThrowIfFailed(DirectXContext::Get()->m_Factory->CreateSwapChain(
            DirectXContext::Get()->m_CommandObject->GetCommandQueue().Get(),
            &swapchainInfo, 
            m_Swapchain.GetAddressOf()));

    	CreateRtvAndDsvDescriptorHeaps();
    }

    ID3D12Resource* DirectXSwapchain::GetCurrentBackBuffer() const
    {
		return m_SwapchainBuffers[m_CurrentBackBuffer].Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DirectXSwapchain::GetCurrentBackBufferView() const
    {
    	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_RenderTargetDescriptor->GetCPUDescriptorHandleForHeapStart(),
			m_CurrentBackBuffer,
			m_RenderTargetDescriptorSize);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DirectXSwapchain::GetDepthStencilView() const
    {
		return m_DepthStencilDescriptor->GetCPUDescriptorHandleForHeapStart();
    }

    void DirectXSwapchain::Resize(uint32_t pWidth, uint32_t pHeight)
    {
    	// Flush before changing any resources.
		DirectXContext::Get()->m_CommandObject->Flush();

	    ThrowIfFailed(DirectXContext::Get()->m_CommandObject->ResetList(DirectXContext::Get()->m_CommandObject->GetCommandAllocator()));

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
	 
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RenderTargetDescriptor->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < k_SwapChainBufferCount; i++)
		{
			ThrowIfFailed(m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&m_SwapchainBuffers[i])));
			DirectXContext::Get()->m_Device->CreateRenderTargetView(m_SwapchainBuffers[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, m_RenderTargetDescriptorSize);
		}

	    // Create the depth/stencil buffer and view.
	    D3D12_RESOURCE_DESC depthStencilDesc;
	    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	    depthStencilDesc.Alignment = 0;
	    depthStencilDesc.Width = pWidth;
	    depthStencilDesc.Height = pHeight;
	    depthStencilDesc.DepthOrArraySize = 1;
	    depthStencilDesc.MipLevels = 1;  
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	    depthStencilDesc.SampleDesc.Count = DirectXContext::Get()->m_4xMsaaState ? 4 : 1;
	    depthStencilDesc.SampleDesc.Quality = DirectXContext::Get()->m_4xMsaaState ? (DirectXContext::Get()->m_4xMsaaQuality - 1) : 0;
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
	    DirectXContext::Get()->m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, GetDepthStencilView());

	    // Transition the resource from its initial state to be used as a depth buffer.
    	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		DirectXContext::Get()->m_CommandObject->GetCommandList()->ResourceBarrier(1, &barrier);
		
	    // Execute the resize commands.
	    DirectXContext::Get()->m_CommandObject->Execute();

		// Wait until resize is complete.
		DirectXContext::Get()->m_CommandObject->Flush();

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
    	DirectXContext::Get()->m_CommandObject->Flush();
    }

	void DirectXSwapchain::CreateRtvAndDsvDescriptorHeaps()
    {
    	m_RenderTargetDescriptorSize = DirectXContext::Get()->m_Device->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    	m_DepthStencilDescriptorSize = DirectXContext::Get()->m_Device->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    	
    	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    	rtvHeapDesc.NumDescriptors = k_SwapChainBufferCount;
    	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    	rtvHeapDesc.NodeMask = 0;
    	ThrowIfFailed(DirectXContext::Get()->m_Device->CreateDescriptorHeap(
			&rtvHeapDesc, IID_PPV_ARGS(m_RenderTargetDescriptor.GetAddressOf())));


    	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    	dsvHeapDesc.NumDescriptors = 1;
    	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    	dsvHeapDesc.NodeMask = 0;
    	ThrowIfFailed(DirectXContext::Get()->m_Device->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(m_DepthStencilDescriptor.GetAddressOf())));
    }
}

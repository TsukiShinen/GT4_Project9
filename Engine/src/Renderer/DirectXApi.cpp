#include "DirectXApi.h"

#include <iostream>

#include "DirectXCommandObject.h"
#include "DirectXCamera.h"
#include "DirectXContext.h"
#include "DirectXSwapchain.h"
#include "Core/Application.h"
#include "Resource/DirectXResourceManager.h"

namespace Engine
{
	void DirectXApi::Initialize()
	{
		InitializeDebug();
		DirectXContext::Initialize();
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
		THROW_IF_FAILED(cmdListAlloc->Reset());
		THROW_IF_FAILED(DirectXContext::Get()->m_CommandObject->ResetList(cmdListAlloc));

		DirectXContext::Get()->m_CommandObject->GetCommandList()->RSSetViewports(
			1, &DirectXContext::Get()->m_Swapchain->GetViewport());
		DirectXContext::Get()->m_CommandObject->GetCommandList()->RSSetScissorRects(
			1, &DirectXContext::Get()->m_Swapchain->GetScissorRect());

		// Indicate a state transition on the resource usage.
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			DirectXContext::Get()->m_Swapchain->GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		DirectXContext::Get()->m_CommandObject->GetCommandList()->ResourceBarrier(1, &barrier);

		// Clear the back buffer and depth buffer.
		DirectXContext::Get()->m_CommandObject->GetCommandList()->ClearRenderTargetView(
			DirectXContext::Get()->m_Swapchain->GetCurrentBackBufferView(), DirectX::Colors::Gray, 0, nullptr);
		DirectXContext::Get()->m_CommandObject->GetCommandList()->ClearDepthStencilView(
			DirectXContext::Get()->m_Swapchain->GetDepthStencilView(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		DirectXContext::Get()->m_ResourceManager->BindDescriptorsHeap();

		const auto renderTargetDescriptor = DirectXContext::Get()->m_Swapchain->GetCurrentBackBufferView();
		const auto depthStencilDescriptor = DirectXContext::Get()->m_Swapchain->GetDepthStencilView();
		DirectXContext::Get()->m_CommandObject->GetCommandList()->OMSetRenderTargets(1,
			&renderTargetDescriptor, true,
			&depthStencilDescriptor);

		DirectXContext::Get()->m_Camera->Update();

		// --- TODO : Refactor this !!!
		DirectXContext::Get()->CurrentFrameData().SetViewProj(DirectXContext::Get()->m_Camera->m_ViewProjT);
		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, DirectXContext::Get()->m_Camera->m_Transform->GetPosition());
		DirectXContext::Get()->CurrentFrameData().SetEyePosition(pos);
		auto light = DirectionalLight();
		light.Direction = {0.57735f, -0.57735f, 0.57735f};
		light.Color = {0.0f, 0.0f, 1.0f};
		DirectXContext::Get()->CurrentFrameData().SetDirectionalLight(0, light);
		auto light2 = DirectionalLight();
		light2.Direction = { -0.57735f, -0.57735f, 0.57735f };
		light2.Color = { 0.5f, 0.0f, 0.0f };
		DirectXContext::Get()->CurrentFrameData().SetDirectionalLight(1, light2);
		DirectXContext::Get()->CurrentFrameData().SetNumDirectionalLights(2);
		// ---

		DirectXContext::Get()->CurrentFrameData().Update();
	}

	void DirectXApi::EndFrame()
	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			DirectXContext::Get()->m_Swapchain->GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		DirectXContext::Get()->m_CommandObject->GetCommandList()->ResourceBarrier(1, &barrier);

		DirectXContext::Get()->m_CommandObject->Execute();
		DirectXContext::Get()->m_Swapchain->Present();

		DirectXContext::Get()->CurrentFrameData().Fence = ++DirectXContext::Get()->m_CommandObject->
			GetCurrentFenceIndex();
		DirectXContext::Get()->m_CommandObject->GetCommandQueue()->Signal(
			DirectXContext::Get()->m_CommandObject->GetFence().Get(),
			DirectXContext::Get()->m_CommandObject->GetCurrentFenceIndex());
	}

	void DirectXApi::UpdateCamera(float dt)
	{
		DirectXContext::Get()->m_Camera->GameUpdate(dt);
	}

	void DirectXApi::CameraMouseEvent(float x, float y)
	{
		DirectXContext::Get()->m_Camera->MouseMove(x, y);
	}

	void DirectXApi::InitializeDebug()
	{
#if defined(DEBUG) || defined(_DEBUG)
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        THROW_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
#endif
	}
}

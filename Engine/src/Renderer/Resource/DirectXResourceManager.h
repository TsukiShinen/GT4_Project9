#pragma once
#include <memory>
#include <queue>
#include <unordered_map>

#include "Texture.h"
#include "Renderer/d3dx12.h"

namespace Engine
{
	class DirectXResourceManager
	{
	public:
		DirectXResourceManager(uint32_t pMaxTextures);
		~DirectXResourceManager();

		void BindDescriptorsHeap() const;

		Texture* LoadTexture(const std::wstring& pPath, const std::string& pName);
		Texture* GetTexture(const std::string& pName) { return m_Textures[pName]; }
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const std::string& pName);
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const Texture* pTexture) const;
		void ReleaseTexture(const std::string& pName);
		void ReleaseTexture(const Texture* pTexture);

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap = nullptr;

		UINT m_CbvSrvDescriptorSize = 0;
		uint32_t m_MaxTextures = 0;
		uint32_t m_TextureCount = 0;
		std::queue<INT> m_TextureIndicesAvailable;
		std::unordered_map<std::string, Texture*> m_Textures;
	};
}

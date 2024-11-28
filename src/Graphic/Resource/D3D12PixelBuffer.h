#pragma once
#include "D3D12Resource.h"
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

class D3D12PixelBuffer
{
public:
	D3D12PixelBuffer() : m_Width(0), m_Height(0), m_ArraySize(0), m_Format(DXGI_FORMAT_UNKNOWN) {}

	uint32_t GetWidth(void) const { return m_Width; }
	uint32_t GetHeight(void) const { return m_Height; }
	uint32_t GetDepth(void) const { return m_ArraySize; }
	const DXGI_FORMAT& GetFormat(void) const { return m_Format; }

	ID3D12Resource* GetResource() const { return ResourceLocation.UnderlyingResource->D3DResource.Get(); }

public:
	TD3D12ResourceLocation ResourceLocation;

protected:
	D3D12_RESOURCE_DESC DescribeTex2D(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize, uint32_t NumMips, DXGI_FORMAT Format, UINT Flags);

	void AssociateWithResource(ID3D12Device* Device, const std::wstring& Name, ID3D12Resource* Resource, D3D12_RESOURCE_STATES CurrentState);

	void CreateTextureResource(ID3D12Device* Device, const std::wstring& Name, const D3D12_RESOURCE_DESC& ResourceDesc,
		D3D12_CLEAR_VALUE ClearValue, D3D12_GPU_VIRTUAL_ADDRESS VidMemPtr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN);

	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_ArraySize;
	DXGI_FORMAT m_Format;
};


class D3D12ColorBuffer : public D3D12PixelBuffer
{
public:
	D3D12ColorBuffer(DirectX::XMFLOAT4 ClearColor = {0.0,0.0,0.0,0.0})
		: m_ClearColor(ClearColor), m_NumMipMaps(0), m_FragmentCount(1), m_SampleCount(1)
	{
		m_RTVHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		m_SRVHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	}

	void CreateFromSwapChain(const std::wstring& name, ID3D12Resource* BaseResource);

	void Create(const std::wstring& name, uint32_t Width, uint32_t Height, uint32_t NumMips, DXGI_FORMAT Format, D3D12_GPU_VIRTUAL_ADDRESS VidMemPtr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN);

	void CreateArray(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t ArrayCount,
		DXGI_FORMAT Format, D3D12_GPU_VIRTUAL_ADDRESS VidMemPtr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN);

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTV(void) const { return m_RTVHandle; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV(void) const { return m_SRVHandle; }

	void SetClearColor(DirectX::XMFLOAT4 ClearColor) { m_ClearColor = ClearColor; }

	DirectX::XMFLOAT4 GetClearClolor() const { return m_ClearColor; }

private:

	D3D12_RESOURCE_FLAGS CombineResourceFlags(void) const
	{
		D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE;

		if (Flags == D3D12_RESOURCE_FLAG_NONE && m_FragmentCount == 1)
			Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		return D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | Flags;
	}

	// Compute the number of texture levels needed to reduce to 1x1.  This uses
	// _BitScanReverse to find the highest set bit.  Each dimension reduces by
	// half and truncates bits.  The dimension 256 (0x100) has 9 mip levels, same
	// as the dimension 511 (0x1FF).
	static inline uint32_t ComputeNumMips(uint32_t Width, uint32_t Height)
	{
		uint32_t HighBit;
		_BitScanReverse((unsigned long*)&HighBit, Width | Height);
		return HighBit + 1;
	}

	void CreateDerivedViews(ID3D12Device* Device, DXGI_FORMAT Format, uint32_t ArraySize, uint32_t NumMips = 1);

	DirectX::XMFLOAT4 m_ClearColor;

	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_SRVHandle;
	uint32_t m_NumMipMaps; // number of texture sublevels
	uint32_t m_FragmentCount;
	uint32_t m_SampleCount;
};


class D3D12DepthBuffer : public D3D12PixelBuffer
{

};

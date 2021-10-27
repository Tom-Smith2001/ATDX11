#pragma once
#include <d3d11.h>

class SwapChain;
class DeviceContext;

class GraphEng
{
public:
	GraphEng();
	//INIT THE GRAPHENG AND DX11 DEVICE
	bool init();
	//RELEASE LOADED RESOURCES
	bool release();
	~GraphEng();

public:
	SwapChain* createSwapChain();
	DeviceContext* getImmediateDeviceContext();
	static GraphEng* get();
	

private:
	DeviceContext* m_imm_device_context;
	ID3D11Device* m_d3d_device;
	D3D_FEATURE_LEVEL m_feature_level;
	//ID3D11DeviceContext* m_imm_context;


	IDXGIDevice* m_dxgi_device;
	IDXGIAdapter* m_dxgi_adapter;
	IDXGIFactory* m_dxgi_factory;
private:
	friend class SwapChain;
};


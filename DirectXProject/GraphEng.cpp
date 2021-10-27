#include "GraphEng.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include <d3dcompiler.h>

GraphEng::GraphEng()
{
}

bool GraphEng::init()
{
	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	UINT num_driver_types = ARRAYSIZE(driver_types);

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};

	UINT num_feature_levels = ARRAYSIZE(feature_levels);

	HRESULT res = 0;

	ID3D11DeviceContext* m_imm_context;

	for (UINT driver_type_index = 0; driver_type_index < num_driver_types; )
	{

		res = D3D11CreateDevice(NULL, driver_types[driver_type_index], NULL, NULL, feature_levels, num_feature_levels, D3D11_SDK_VERSION,
			&m_d3d_device, &m_feature_level, &m_imm_context);

		if (SUCCEEDED(res))
			break;
		
		++driver_type_index;
	}

	if (FAILED(res))
		return false;

	m_imm_device_context = new DeviceContext(m_imm_context);

	m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
	m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
	m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);


	return true;
}

bool GraphEng::release()
{
	m_dxgi_device->Release();
	m_dxgi_adapter->Release();
	m_dxgi_factory->Release();
	m_imm_device_context->release();
	m_d3d_device->Release();
	return true;
}

GraphEng::~GraphEng()
{
}

SwapChain* GraphEng::createSwapChain()
{
	return new SwapChain();
}

DeviceContext* GraphEng::getImmediateDeviceContext()
{
	return this->m_imm_device_context;
}

GraphEng* GraphEng::get()
{
	static GraphEng engine;
	return &engine;
}

VertexBuffer* GraphEng::createVertexBuffer()
{
	return new VertexBuffer();
}

bool GraphEng::createShaders()
{
	ID3DBlob* errblob = nullptr;

	D3DCompileFromFile(L"shader.fx", nullptr, nullptr, "vsmain", "vs_5_0", NULL, NULL, &m_vsblob, &errblob);
	D3DCompileFromFile(L"shader.fx", nullptr, nullptr, "psmain", "ps_5_0", NULL, NULL, &m_psblob, &errblob);


	m_d3d_device->CreateVertexShader(m_vsblob->GetBufferPointer(), m_vsblob->GetBufferSize(), nullptr, &m_vs);
	m_d3d_device->CreatePixelShader(m_psblob->GetBufferPointer(), m_psblob->GetBufferSize(), nullptr, &m_ps);
	return true;
}

bool GraphEng::setShaders()
{
	m_imm_context->VSSetShader(m_vs, nullptr, 0);
	m_imm_context->PSSetShader(m_ps, nullptr, 0);
	return true;
}

void GraphEng::getShaderBufferAndSize(void** bytecode, UINT* size)
{
	*bytecode = this->m_vsblob->GetBufferPointer();
	*size = (UINT)this->m_vsblob->GetBufferSize();
}

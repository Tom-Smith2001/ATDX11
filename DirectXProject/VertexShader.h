#pragma once
#include <d3d11.h>

class GraphEng;
class DeviceContext;

class VertexShader
{
public:
	VertexShader();
	~VertexShader();
	void release();
private:
	bool init(const void* shader_byte_code, size_t byte_code_size);


	ID3D11VertexShader* m_vs;


	friend class GraphEng;
	friend class DeviceContext;
};


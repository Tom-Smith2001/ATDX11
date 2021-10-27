#include "AppWindow.h"



struct vec3
{
	float x, y, z;
};

struct vertex
{
	vec3 position;
};


AppWindow::AppWindow()
{
}


AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	Window::onCreate();

	GraphEng::get()->init();
	m_swap_chain = GraphEng::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	UINT rc_width = rc.right - rc.left;
	UINT rc_height = rc.bottom - rc.top;

	m_swap_chain->init(this->m_hwnd, rc_width, rc_height);

	vertex list[] =
	{
		//X, Y, Z
		{-0.5f, -0.5f, 0.0f}, //#1
		{0.0f, 0.5f, 0.0f}, //#2
		{0.5f, -0.5f, 0.0f}, //#3
	};

	m_vb = GraphEng::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(list);

	GraphEng::get()->createShaders();
	void* shader_byte_code = nullptr;
	UINT size_shader = 0;
	GraphEng::get()->getShaderBufferAndSize(&shader_byte_code, &size_shader);

	m_vb->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	GraphEng::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0.2, 0.5, 1, 1);

	RECT rc = this->getClientWindowRect();
	GraphEng::get()->getImmediateDeviceContext()->setViewPortSize(rc.right - rc.left, rc.bottom - rc.top);
	GraphEng::get()->setShaders();

	GraphEng::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);

	GraphEng::get()->getImmediateDeviceContext()->drawTriangleList(m_vb->getSizeVertexList(), 0);

	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_swap_chain->release();
	GraphEng::get()->release();
}

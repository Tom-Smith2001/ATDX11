#include "AppWindow.h"

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
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	GraphEng::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0, 1, 0, 1);

	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();

	m_swap_chain->release();
	GraphEng::get()->release();
}

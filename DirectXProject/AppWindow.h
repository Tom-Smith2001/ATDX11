#pragma once
#include "Window.h"
#include "GraphEng.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"

class AppWindow: public Window
{
public:
	AppWindow();
	~AppWindow();

	// INHERITED FROM WINDOW
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;
private:
	SwapChain* m_swap_chain;
	VertexBuffer* m_vb;
};


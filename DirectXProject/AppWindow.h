#pragma once
#include "Window.h"
#include "GraphEng.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputListener.h"
#include "Matrix4x4.h"
#include <vector>

struct CubeBounds 
{
	float x_min, x_max, z_min, z_max;
};



class AppWindow: public Window, public InputListener
{
public:
	AppWindow();
	~AppWindow();

	void update();

	// INHERITED FROM WINDOW
	virtual void onCreate() override;
	void BuildMap(int num);
	Matrix4x4 CheckCollisions(Matrix4x4 cam);
	virtual void onUpdate() override;
	virtual void onDestroy() override;
	virtual void onFocus() override;
	virtual void onKillFocus() override;

	virtual void onKeyDown(int key) override;
	virtual void onKeyUp(int key) override;

	virtual void onMouseMove(const Point& mouse_pos) override;
	virtual void onLeftMouseDown(const Point& mouse_pos) override;
	virtual void onLeftMouseUp(const Point& mouse_pos) override;
	virtual void onRightMouseDown(const Point& mouse_pos) override;
	virtual void onRightMouseUp(const Point& mouse_pos) override;
private:
	SwapChain* m_swap_chain;
	VertexBuffer* m_vb;
	VertexShader* m_vs;
	PixelShader* m_ps;
	ConstantBuffer* m_cb;
	IndexBuffer* m_ib;

	long m_old_delta;
	long m_new_delta;
	float m_delta_time;

	float m_delta_pos;
	float m_delta_scale;
	float m_delta_rot;

	float m_rot_x = 0;
	float m_rot_y = 0;


	float m_scale_cube = 1;
	float m_forward = 0.0f;
	float m_strafe = 0.0f;
	Matrix4x4 m_world_cam;

	CubeBounds cube_bounds[100];


	
};


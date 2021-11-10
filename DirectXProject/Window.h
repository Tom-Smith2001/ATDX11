#pragma once
#include <Windows.h>


class Window
{
public:
	Window();
	//INITIALISE THE WINDOW
	bool init();
	//TRANSLATES AND SENDS EVENT MESSAGES TO THE WNDPROC
	bool broadcast();
	//DESTROY THE WINDOW
	bool release();
	bool isRun();

	RECT getClientWindowRect();
	void setHWND(HWND hwnd);

	//EVENTS
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();

	~Window();
protected:
	HWND m_hwnd;
	bool m_is_run;

};


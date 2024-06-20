#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#include "ApplicationClass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows();
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	ApplicationClass* m_applicationClass;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;


#endif
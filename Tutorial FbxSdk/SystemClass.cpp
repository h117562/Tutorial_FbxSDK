#include "Systemclass.h"


SystemClass::SystemClass()
{
	m_applicationClass = 0;
}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int screenwidth, screenheight;
	int posX, posY;
	bool result;

	//메시지 핸들
	ApplicationHandle = this;

	//윈도우 핸들 가져오기
	m_hinstance = GetModuleHandle(NULL);

	//윈도우 이름
	m_applicationName = L"DWrite Tutorial";

	//윈도우 클래스 설정
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//윈도우 클래스 등록
	RegisterClassEx(&wc);

	//스크린크기
	screenwidth = 1200;
	screenheight = 800;

	//화면 중앙에 정렬
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenwidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenheight) / 2;


	//프로그램 윈도우 생성
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_OVERLAPPEDWINDOW,
		posX, posY, screenwidth, screenheight, NULL, NULL, m_hinstance, NULL);

	//윈도우 포커스 가져오기
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	m_applicationClass = new ApplicationClass();
	if (!m_applicationClass)
	{
		result = false;
	}

	result = m_applicationClass->Initialize(m_hinstance, m_hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	return result;
}


void SystemClass::Shutdown()
{

	if (m_applicationClass)
	{
		m_applicationClass->Shutdown();
		delete m_applicationClass;
		m_applicationClass = 0;
	}

	//윈도우 관련 자원반환
	ShutdownWindows();

	return;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	//변수 초기화
	ZeroMemory(&msg, sizeof(MSG));


	done = false;
	while (!done)
	{
		//메시지 전달확인
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//종료 메시지 확인
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//종료가 아니면 사이클반복
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}

bool SystemClass::Frame()
{
	bool result;

	result = m_applicationClass->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::ShutdownWindows()
{

	//핸들 삭제
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//윈도우 클래스 등록해제
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//핸들 반환
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
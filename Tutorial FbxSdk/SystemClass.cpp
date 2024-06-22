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

	//�޽��� �ڵ�
	ApplicationHandle = this;

	//������ �ڵ� ��������
	m_hinstance = GetModuleHandle(NULL);

	//������ �̸�
	m_applicationName = L"DWrite Tutorial";

	//������ Ŭ���� ����
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

	//������ Ŭ���� ���
	RegisterClassEx(&wc);

	//��ũ��ũ��
	screenwidth = 1200;
	screenheight = 800;

	//ȭ�� �߾ӿ� ����
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenwidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenheight) / 2;


	//���α׷� ������ ����
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_OVERLAPPEDWINDOW,
		posX, posY, screenwidth, screenheight, NULL, NULL, m_hinstance, NULL);

	//������ ��Ŀ�� ��������
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

	//������ ���� �ڿ���ȯ
	ShutdownWindows();

	return;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	//���� �ʱ�ȭ
	ZeroMemory(&msg, sizeof(MSG));


	done = false;
	while (!done)
	{
		//�޽��� ����Ȯ��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//���� �޽��� Ȯ��
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//���ᰡ �ƴϸ� ����Ŭ�ݺ�
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

	//�ڵ� ����
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//������ Ŭ���� �������
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//�ڵ� ��ȯ
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
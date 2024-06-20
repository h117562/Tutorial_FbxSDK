#include "InfoUiClass.h"

InfoUiClass::InfoUiClass()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = 0;
}

bool InfoUiClass::Initialize(D3DClass* d3dClass)
{
	bool result;
	char videoCard[128];
	int videoMemory;

	//�׷��� ī�� �̸���, �޸� ũ�⸦ ������
	d3dClass->GetVideoCardInfo(videoCard, videoMemory);

	//���� �ʱ�ȭ
	wcscpy_s(m_videoNameString, L"");
	MultiByteToWideChar(CP_ACP, 0, videoCard, strlen(videoCard) + 1, m_videoNameString, _countof(m_videoNameString));
	//wchar_t�� Itoa, ������ �Ű������� 10������ ���� �⺻ 16�����̹Ƿ� �� 10 ����� ��
	_itow_s(videoMemory, m_tempString, _countof(m_tempString), 10); 

	wcscpy_s(m_memoryString, L"VRAM: ");
	wcscat_s(m_memoryString, m_tempString);
	wcscat_s(m_memoryString, L" MB");

	//���� �ʱ�ȭ
	m_previousPos = XMINT3(0, 0, 0);
	m_previousRot = XMINT3(0, 0, 0);

	wcscpy_s(m_stringPX, L"PX: 0");
	wcscpy_s(m_stringPY, L"PY: 0");
	wcscpy_s(m_stringPZ, L"PZ: 0");

	wcscpy_s(m_stringRX, L"RX: 0");
	wcscpy_s(m_stringRY, L"RY: 0");
	wcscpy_s(m_stringRZ, L"RZ: 0");

	return true;
}

void InfoUiClass::Frame(TextClass* m_textclass, XMFLOAT3 pos, XMFLOAT3 rot)
{
	XMFLOAT3 position = pos;
	XMFLOAT3 rotation = rot;

	m_position = XMINT3((int)position.x, (int)position.y, (int)position.z);
	m_rotation = XMINT3((int)rotation.x, (int)rotation.y, (int)rotation.z);

	GetFps();

	//Fps�� ���
	wcscpy_s(m_fpsString, L"FPS: ");
	_itow_s(m_fps, m_tempString, _countof(m_tempString), 10);
	wcscat_s(m_fpsString, m_tempString);
	m_textclass->RenderText(m_fpsString, 0, 0, 800, 400);


	//���� ī�� ������ ���
	m_textclass->RenderText(m_videoNameString, 0, 30, 800, 400);
	m_textclass->RenderText(m_memoryString, 0, 60, 800, 400);


	//�� ������ ���� ��쿡�� �ؽ�Ʈ �ʱ�ȭ
	if (m_position.x != m_previousPos.x)
	{
		wcscpy_s(m_stringPX, L"PX: ");
		_itow_s(m_position.x, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringPX, m_tempString);
		m_previousPos.x = m_position.x;
	}

	if (m_position.y != m_previousPos.y)
	{
		wcscpy_s(m_stringPY, L"PY: ");
		_itow_s(m_position.y, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringPY, m_tempString);
		m_previousPos.y = m_position.y;
	}

	if (m_position.z != m_previousPos.z)
	{
		wcscpy_s(m_stringPZ, L"PZ: ");
		_itow_s(m_position.z, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringPZ, m_tempString);
		m_previousPos.z = m_position.z;
	}

	if (m_rotation.x != m_previousRot.x)
	{
		wcscpy_s(m_stringRX, L"RX: ");
		_itow_s(m_rotation.x, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringRX, m_tempString);
		m_rotation.x = m_previousRot.x;
	}

	if (m_rotation.y != m_previousRot.y)
	{
		wcscpy_s(m_stringRY, L"RY: ");
		_itow_s(m_rotation.y, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringRY, m_tempString);
		m_rotation.y = m_previousRot.y;
	}

	if (m_rotation.z != m_previousRot.z)
	{
		wcscpy_s(m_stringRZ, L"RZ: ");
		_itow_s(m_rotation.z, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringRZ, m_tempString);
		m_rotation.z = m_previousRot.z;
	}

	//ī�޶� ��ġ ������ ���
	m_textclass->RenderText(m_stringPX, 0, 120, 800, 400);
	m_textclass->RenderText(m_stringPY, 0, 150, 800, 400);
	m_textclass->RenderText(m_stringPZ, 0, 180, 800, 400);
	m_textclass->RenderText(m_stringRX, 0, 210, 800, 400);
	m_textclass->RenderText(m_stringRY, 0, 240, 800, 400);
	m_textclass->RenderText(m_stringRZ, 0, 270, 800, 400);


	return;
}

void InfoUiClass::GetFps()
{
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}

	return;
}

void InfoUiClass::Shutdown()
{
	return;
}
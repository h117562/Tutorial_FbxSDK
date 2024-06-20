#ifndef _INFO_UICLASS_H_
#define _INFO_UICLASS_H_

#include <Windows.h>
#include "textclass.h"

class InfoUiClass
{
public:
	InfoUiClass();

	bool Initialize(D3DClass*);
	void Frame(TextClass* , XMFLOAT3, XMFLOAT3);
	void Shutdown();
private:
	void GetFps();

private:
	int m_fps, m_count;
	wchar_t m_fpsString[20];
	wchar_t m_videoNameString[144];
	wchar_t m_memoryString[32];
	wchar_t m_tempString[16];
	unsigned long m_startTime;

	XMINT3 m_position, m_rotation;
	XMINT3 m_previousPos, m_previousRot;
	wchar_t m_stringPX[16], m_stringPY[16], m_stringPZ[16];
	wchar_t m_stringRX[16], m_stringRY[16], m_stringRZ[16];

	};

#endif
#include "Systemclass.h"
#pragma comment(linker , "/entry:WinMainCRTStartup /subsystem:console")
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;


	//SystemClass 생성
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	//SystemClass 구현
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	//모든 자원 반환
	System->Shutdown();
	delete System;
	System = 0;


	return 0;
}
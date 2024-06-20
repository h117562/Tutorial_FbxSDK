#include "Systemclass.h"
#pragma comment(linker , "/entry:WinMainCRTStartup /subsystem:console")
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;


	//SystemClass ����
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	//SystemClass ����
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	//��� �ڿ� ��ȯ
	System->Shutdown();
	delete System;
	System = 0;


	return 0;
}
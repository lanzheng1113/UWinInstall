#include "stdafx.h"

void __cdecl ThreadFunExtraSetup(void* ThreadParam)
{
	Sleep(1000);
	PostQuitMessage(0);
	return;
}
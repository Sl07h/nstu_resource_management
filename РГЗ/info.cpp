#define _CRT_SECURE_NO_WARNINGS
#define UNICODE
#include <stdio.h>
#include <Windows.h>


// Функция динамической библиотеки
extern "C" _declspec(dllexport) void Information(int& maxWidth, bool& isMMXSupported)
{
	// Определяем максимальную ширину окна (в пикселях)
	maxWidth = GetSystemMetrics(SM_CXFULLSCREEN);

	// Определяем наличие поддержки MMX технологии
	int fSupported;
	__asm
	{
		mov eax, 1 // CPUID уровня 1
		cpuid
		and edx, 0x800000 // проверяем 23 бит регистра EDX
		mov fSupported, edx // если MMX поддерживается, то EDX[28] == 1
	}
	if (fSupported != 0)
		isMMXSupported = true;
}
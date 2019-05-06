#define _CRT_SECURE_NO_WARNINGS
#define UNICODE
#include <stdio.h>
#include <Windows.h>


// Функция динамической библиотеки
extern "C" _declspec(dllexport) int Information(int &year, bool &isAssociative)
{
	// Узнаём текущий год
	SYSTEMTIME time;
	GetSystemTime(&time);
	year = time.wYear;

	// Определяем ассоциативность КЭШа 1 уровня
	isAssociative = true;
	return 0;
}
#define _CRT_SECURE_NO_WARNINGS
#define UNICODE
#include <windows.h>
#include <string>
#include <stdio.h>
#include <Windows.h>


LPCWSTR		szClassName = L"MyClass";
LPCWSTR		szTitle = L"Индивидуальное задание по УРВС";
TCHAR		YearInfo[256];
TCHAR		L1CashInfo[256];


// Функция выполняющаяся в отдельном потоке
DWORD WINAPI ThreadFunc(void *)
{
	// Подключаем динамическую библиотеку
	typedef int(*ImportFunction)(int &, bool &);
	ImportFunction DLLInfo;
	HINSTANCE hinstLib = LoadLibrary(TEXT("info.dll"));
	DLLInfo = (ImportFunction)GetProcAddress(hinstLib, "Information");


	int currentYear = -9000;
	bool isAssociative = false;
	DLLInfo(currentYear, isAssociative);
	FreeLibrary(hinstLib);

	// Интерпретация выходных данных функции DLLInfo
	wsprintf(YearInfo, L"Текущий год: %d", currentYear);
	if (isAssociative)
		wsprintf(L1CashInfo, L"КЭШ первого уровня ассоциативен");
	else
		wsprintf(L1CashInfo, L"КЭШ первого уровня неассоциативен");

	return 0;
}


// Процедура обработки сообщений
LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	HFONT hFont = CreateFont(18, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
							CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY, VARIABLE_PITCH, TEXT("Calibri"));

	switch (msg)
	{
		// Создание потока
		case WM_CREATE:
			HANDLE hThread;
			DWORD IDThread;
			hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
			break;

		// Закрытие окна
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		// Инициализация контекста устройства
		case WM_PAINT:
		
			hDC = BeginPaint(hWnd, &ps);
			SelectObject(hDC, hFont);				// Меняем шрифт
			TextOut(hDC, 15, 17, YearInfo, 256);
			TextOut(hDC, 15, 42, L1CashInfo, 256);
			EndPaint(hWnd, &ps);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}


// Главная функция Windows-приложения
int WINAPI WinMain(HINSTANCE hThisInst,
	HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
	MSG msg;
	WNDCLASS wcl;
	HWND hWnd;

	// Создание класса окна
	wcl.hInstance		= hThisInst;
	wcl.lpszClassName	= szClassName;
	wcl.lpfnWndProc		= WindowFunc;
	wcl.style			= CS_HREDRAW | CS_VREDRAW;
	wcl.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcl.lpszMenuName	= NULL;
	wcl.cbClsExtra		= 0;
	wcl.cbWndExtra		= 0;
	wcl.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	// Регистрация класса окна
	RegisterClass(&wcl);
	// Создание окна на базе его класса
	hWnd = CreateWindow(szClassName, szTitle,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS,
		100, 50, 700, 120, HWND_DESKTOP,
		NULL, hThisInst, NULL);
	// Отображение окна
	ShowWindow(hWnd, nWinMode);
	UpdateWindow(hWnd);
	// Цикл обработки сообщений
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
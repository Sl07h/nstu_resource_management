#define _CRT_SECURE_NO_WARNINGS
#define UNICODE
#include <windows.h>
#include <string>
#include <stdio.h>
#include <Windows.h>


LPCWSTR		szClassName = L"MyClass";
LPCWSTR		szTitle = L"Individual work - resource managment";
TCHAR		ScreenInfo[256];
TCHAR		MMXInfo[256];

// Функция выполняющаяся в отдельном потоке
DWORD WINAPI ThreadFunc(void *)
{
	typedef int(*ImportFunction)(int&, bool&);
	ImportFunction DLLInfo; // Подключаем динамическую библиотеку
	HINSTANCE hinstLib = LoadLibrary(TEXT("info.dll"));
	DLLInfo = (ImportFunction)GetProcAddress(hinstLib, "Information");

	int maxWidth = 0;
	bool isMMXSupported = false;
	DLLInfo(maxWidth, isMMXSupported); // Получаем данные
	FreeLibrary(hinstLib); // Высвобождаем библиотеку

	// Интерпретация выходных данных функции DLLInfo
	wsprintf(ScreenInfo, L"Max screen width is: %d px", maxWidth);
	if (isMMXSupported)
		wsprintf(MMXInfo, L"MMX techology is supported");
	else
		wsprintf(MMXInfo, L"MMX techology isn't supported");

	return 0;
}


// Процедура обработки сообщений
LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	// Создание шрифтра
	HFONT hFont = CreateFont(20, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
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
			SelectObject(hDC, hFont); // Меняем шрифт
			TextOut(hDC, 15, 10, ScreenInfo, 256);
			TextOut(hDC, 15, 30, MMXInfo, 256);
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
	wcl.hInstance			= hThisInst;
	wcl.lpszClassName		= szClassName;
	wcl.lpfnWndProc			= WindowFunc;
	wcl.style				= CS_HREDRAW | CS_VREDRAW;
	wcl.hIcon				= LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor				= LoadCursor(NULL, IDC_ARROW);
	wcl.lpszMenuName		= NULL;
	wcl.cbClsExtra			= 0;
	wcl.cbWndExtra			= 0;
	wcl.hbrBackground		= (HBRUSH)GetStockObject(WHITE_BRUSH);

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
#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN

#include <iostream>

#include <windows.h>
#include <ShellScalingApi.h>

constexpr unsigned int WIDTH = 640;
constexpr unsigned int HEIGHT = 480;

auto EXIT_PROGRAM = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	// create window and draw in it
	WNDCLASS window_class = {};
	window_class.style = CS_VREDRAW | CS_HREDRAW;
	window_class.lpszClassName = CLASS_NAME;
	window_class.lpfnWndProc = WindowProc;

	if (!RegisterClass(&window_class))
	{
		MessageBox(NULL, L"Failed To Register the window class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		exit(1);
	}

	RECT client_rect = {
		0,
		0,
		WIDTH,
		HEIGHT
	};

	if (!AdjustWindowRect(&client_rect, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, false))
	{
		std::cout << "ERROR: Adjusting window rectangle failed." << std::endl;
	}

	// disable automatic rescaling 
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,     // Window class
		L"Minimal Direct3D Example",    // Window text
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,    // prevent resizing
		CW_USEDEFAULT, CW_USEDEFAULT,
		client_rect.right - client_rect.left,
		client_rect.bottom - client_rect.top,
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		printf("ERROR: hwnd == NULL\n");
		return 0;
	}

	ShowWindow(hwnd, SW_SHOW);

	// Run the message loop.
	while (!EXIT_PROGRAM)
	{
		MSG msg = { };
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		EXIT_PROGRAM = true;
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		//FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <exception>

#include <windows.h>
#include <ShellScalingApi.h>

// Direct3D header
#include <d3d11.h>
#include <DirectXPackedVector.h>


namespace DX
{
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		// Set a breakpoint on this line to catch DirectX API errors
		throw std::exception();
	}
}
}

// include the Direct3D Library file
//#pragma comment (lib, "d3d11.lib")

// global declarations
IDXGISwapChain* swapchain;             
ID3D11Device* dev;                     
ID3D11DeviceContext* devcon;
ID3D11RenderTargetView* backbuffer;

constexpr unsigned int WIDTH = 640;
constexpr unsigned int HEIGHT = 480;

auto EXIT_PROGRAM = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool init_d3d(HWND hWnd);
void cleanup();

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

	// start Direct3D
	init_d3d(hwnd);

	ShowWindow(hwnd, SW_SHOW);

	// run the message loop
	while (!EXIT_PROGRAM)
	{
		float clear_color[4] = {
			0.0, //red
			0.0, //green
			0.0, //blue
			1.0 }; //alpha

		// clear the back buffer to a deep blue
		devcon->ClearRenderTargetView(backbuffer, clear_color);

		// do 3D rendering on the back buffer here

		// switch the back buffer and the front buffer
		swapchain->Present(0, 0);

		MSG msg = { };
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	cleanup();
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

bool init_d3d(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd = {};

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

	// create a device, device context and swap chain using the information in the scd struct
	DX::ThrowIfFailed(D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		nullptr,
		&devcon));

	// get the address of the back buffer
	ID3D11Texture2D* pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	DX::ThrowIfFailed(dev->CreateRenderTargetView(pBackBuffer, nullptr, &backbuffer));
	pBackBuffer->Release();

	// set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, nullptr);

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = WIDTH;
	viewport.Height = HEIGHT;

	devcon->RSSetViewports(1, &viewport);

	return true;
}

void cleanup()
{
	swapchain->Release();
	dev->Release();
	devcon->Release();
	backbuffer->Release();
}
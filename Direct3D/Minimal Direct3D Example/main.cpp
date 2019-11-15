#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <exception>

#include <windows.h>
#include <ShellScalingApi.h> // for correct display scaling detection

// Direct3D headers
#include <d3d11.h>
#include <d3dcompiler.h>

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

class GraphicsState
{
public:
	ID3D11Device* dev;
	ID3D11DeviceContext* devcon;
	IDXGISwapChain* swapchain;
	ID3D11RenderTargetView* backbuffer;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11RasterizerState* rasterState;

	void init(HWND hWnd);
	void cleanup();
};

static constexpr UINT WIDTH = 800;
static constexpr UINT HEIGHT = 600;

static auto EXIT_PROGRAM = false;

// callback function for the created window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// helper function for compiling HLSL code
HRESULT compileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, 
	_In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);

// entry point for graphical apps on Windows
int WINAPI wWinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPWSTR lpCmdLine, 
	_In_ int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"Window Class";

	HWND hwnd;
	WNDCLASS windowClass = {};
	std::shared_ptr<GraphicsState> graphState(new GraphicsState());

	RECT clientRect;

	// create window and draw in it
	windowClass.style = CS_VREDRAW | CS_HREDRAW;
	windowClass.lpszClassName = CLASS_NAME;
	windowClass.lpfnWndProc = WindowProc;

	if (!RegisterClass(&windowClass))
	{
		MessageBox(NULL, L"Failed To Register the window class.", L"ERROR", 
			MB_OK | MB_ICONEXCLAMATION);
		exit(1);
	}

	// disable automatic rescaling 
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

	clientRect = {
		0,
		0,
		WIDTH,
		HEIGHT
	};

	if (!AdjustWindowRect(&clientRect, WS_SYSMENU | WS_MINIMIZEBOX | 
		WS_MAXIMIZEBOX | WS_CAPTION, false))
	{
		std::cout << "ERROR: Adjusting window rectangle failed." << std::endl;
	}
	
	hwnd = CreateWindowEx(
		0, // optional window styles.
		CLASS_NAME,	// window class
		L"Minimal Direct3D Example", // window text
		WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION , // prevent resizing
		CW_USEDEFAULT, // display screen x-coordinate of starting position
		CW_USEDEFAULT, // display screen y-coordinate of starting position
		clientRect.right - clientRect.left, // width of the to be created window
		clientRect.bottom - clientRect.top, // height of the to be created window
		nullptr, // parent window    
		nullptr, // menu
		hInstance, // instance handle
		nullptr	// additional application data
	);
	
	if (hwnd == NULL)
	{
		printf("ERROR: hwnd == NULL\n");
		return 0;
	}

	// start Direct3D
	graphState->init(hwnd);

	ID3DBlob* vshaderBlob;
	ID3DBlob* pshaderBlob;
	ID3D11VertexShader* vshader;
	ID3D11PixelShader* pshader;

	DX::ThrowIfFailed(compileShader(L"../../../Minimal Direct3D Example/shaders/vertexShader.hlsl",
		"main",
		"vs_5_0",
		&vshaderBlob));
	DX::ThrowIfFailed(compileShader(L"../../../Minimal Direct3D Example/shaders/pixelShader.hlsl",
		"main",
		"ps_5_0",
		&pshaderBlob));

	DX::ThrowIfFailed(graphState->dev->CreateVertexShader(
		vshaderBlob->GetBufferPointer(),
		vshaderBlob->GetBufferSize(), NULL, &vshader));
	DX::ThrowIfFailed(graphState->dev->CreatePixelShader(
		pshaderBlob->GetBufferPointer(),
		pshaderBlob->GetBufferSize(), NULL, &pshader));

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vshaderBlob->Release();
	vshaderBlob = nullptr;

	pshaderBlob->Release();
	pshaderBlob = nullptr;

	// Set the vertex and pixel shaders that will be used to render this triangle.
	graphState->devcon->VSSetShader(vshader, nullptr, 0);
	graphState->devcon->PSSetShader(pshader, nullptr, 0);

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);

	float clear_color[4] = {
			0.0, //red
			0.3, //green
			0.3, //blue
			1.0  //alpha
	};
	//UINT stride = 12;
	//UINT offset = 0;

	////devcon->IASetInputLayout(m_layout);
	//// Set the vertex buffer to active in the input assembler so it can be rendered.
	//devcon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	//// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	//devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphState->devcon->OMSetRenderTargets(1, &(graphState->backbuffer),
		nullptr);

	graphState->devcon->IASetInputLayout(nullptr);
	graphState->devcon->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	graphState->devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// run the message loop
	while (!EXIT_PROGRAM)
	{
		// do 3D rendering on the back buffer here
		graphState->devcon->ClearRenderTargetView(graphState->backbuffer,
			clear_color);
		// Render the triangle.
		graphState->devcon->Draw(6, 0);

		// switch the back buffer and the front buffer
		graphState->swapchain->Present(0, 0);

		MSG msg = { };
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	graphState->cleanup();
	return 0;
}

LRESULT CALLBACK WindowProc(
	HWND hwnd, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		EXIT_PROGRAM = true;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void GraphicsState::init(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd = {};
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ID3D11Texture2D* pBackBuffer;
	D3D11_VIEWPORT viewport = {};

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
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		nullptr,
		&devcon));

	// get the address of the back buffer
	DX::ThrowIfFailed(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));

	// use the back buffer address to create the render target
	DX::ThrowIfFailed(dev->CreateRenderTargetView(pBackBuffer, nullptr, &backbuffer));
	pBackBuffer->Release();

	// set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, nullptr);

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	DX::ThrowIfFailed(dev->CreateRasterizerState(&rasterDesc, &rasterState));
	
	// Now set the rasterizer state.
	devcon->RSSetState(rasterState);

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = WIDTH;
	depthBufferDesc.Height = HEIGHT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	DX::ThrowIfFailed(dev->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer));

	// Set the viewport
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = WIDTH;
	viewport.Height = HEIGHT;

	devcon->RSSetViewports(1, &viewport);
}

// release all acquired resources
void GraphicsState::cleanup()
{
	swapchain->Release();
	dev->Release();
	devcon->Release();
	backbuffer->Release();
	rasterState->Release();
	depthStencilBuffer->Release();
}

HRESULT compileShader(
	_In_ LPCWSTR srcFile,
	_In_ LPCSTR entryPoint,
	_In_ LPCSTR profile,
	_Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}
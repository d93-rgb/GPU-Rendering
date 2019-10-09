#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <exception>

#include <windows.h>
#include <ShellScalingApi.h>

// Direct3D header
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

// include the Direct3D Library file
//#pragma comment (lib, "d3d11.lib")

// global declarations
class GraphicsState
{
public:
	ID3D11Device* dev;
	ID3D11DeviceContext* devcon;
	IDXGISwapChain* swapchain;
	ID3D11RenderTargetView* backbuffer;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11RasterizerState* raster_state;

	bool init(HWND hWnd);
	void cleanup();
};

constexpr unsigned int WIDTH = 800;
constexpr unsigned int HEIGHT = 600;

static auto EXIT_PROGRAM = false;

// callback function for the created window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HRESULT compile_shader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, 
	_In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPWSTR lpCmdLine, 
	_In_ int nCmdShow)
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

	// disable automatic rescaling 
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

	RECT client_rect = {
		0,
		0,
		WIDTH,
		HEIGHT
	};

	if (!AdjustWindowRect(&client_rect, WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION, false))
	{
		std::cout << "ERROR: Adjusting window rectangle failed." << std::endl;
	}
	
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,     // Window class
		L"Minimal Direct3D Example",    // Window text
		WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION ,    // prevent resizing
		CW_USEDEFAULT, CW_USEDEFAULT,
		client_rect.right - client_rect.left,
		client_rect.bottom - client_rect.top,
		nullptr,       // Parent window    
		nullptr,       // Menu
		hInstance,  // Instance handle
		nullptr	// Additional application data
	);
	/*SetWindowPos(hwnd, NULL, 0, 0, client_rect.right - client_rect.left, 
		client_rect.bottom - client_rect.top, SWP_NOZORDER | SWP_NOMOVE);*/

	if (hwnd == NULL)
	{
		printf("ERROR: hwnd == NULL\n");
		return 0;
	}

	// start Direct3D
	init_d3d(hwnd);

	ID3DBlob* vshader_blob;
	ID3DBlob* pshader_blob;
	ID3D11VertexShader* vshader;
	ID3D11PixelShader* pshader;

	DX::ThrowIfFailed(compile_shader(L"../../../Minimal Direct3D Example/shaders/vertexShader.hlsl",
		"main",
		"vs_5_0",
		&vshader_blob));
	DX::ThrowIfFailed(compile_shader(L"../../../Minimal Direct3D Example/shaders/pixelShader.hlsl",
		"main",
		"ps_5_0",
		&pshader_blob));

	DX::ThrowIfFailed(dev->CreateVertexShader(vshader_blob->GetBufferPointer(), 
		vshader_blob->GetBufferSize(), NULL, &vshader));
	DX::ThrowIfFailed(dev->CreatePixelShader(pshader_blob->GetBufferPointer(),
		pshader_blob->GetBufferSize(), NULL, &pshader));

	//D3D11_INPUT_ELEMENT_DESC polygonLayout;
	//polygonLayout.SemanticName = "POSITION";
	//polygonLayout.SemanticIndex = 0;
	//polygonLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//polygonLayout.InputSlot = 0;
	//polygonLayout.AlignedByteOffset = 0;
	//polygonLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//polygonLayout.InstanceDataStepRate = 0;

	//// Get a count of the elements in the layout.
	////UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	//UINT numElements = 1;

	//ID3D11InputLayout* m_layout;
	//// Create the vertex input layout.
	//DX::ThrowIfFailed(dev->CreateInputLayout(&polygonLayout, numElements, vshader_blob->GetBufferPointer(),
	//	vshader_blob->GetBufferSize(), &m_layout));

	//ID3D11Buffer* m_vertexBuffer;
	//D3D11_BUFFER_DESC vertexBufferDesc;
	//D3D11_SUBRESOURCE_DATA vertexData;

	//float vertices[] = {
	//	1.0, 0.0, 0.0,
	//	-1.0, 0.0, 0.0,
	//	0.0, 1.0, 0.0
	//};

	//// Set up the description of the static vertex buffer.
	//vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//vertexBufferDesc.ByteWidth = sizeof(vertices);
	//vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vertexBufferDesc.CPUAccessFlags = 0;
	//vertexBufferDesc.MiscFlags = 0;
	//vertexBufferDesc.StructureByteStride = 0;

	//// Give the subresource structure a pointer to the vertex data.
	//vertexData.pSysMem = vertices;
	//vertexData.SysMemPitch = 0;
	//vertexData.SysMemSlicePitch = 0;

	//DX::ThrowIfFailed(dev->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vshader_blob->Release();
	vshader_blob = nullptr;

	pshader_blob->Release();
	pshader_blob = nullptr;

	// Set the vertex and pixel shaders that will be used to render this triangle.
	devcon->VSSetShader(vshader, nullptr, 0);
	devcon->PSSetShader(pshader, nullptr, 0);

	D3D11_RASTERIZER_DESC rasterizer_desc;
	ID3D11RasterizerState* rasterizer_state;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = false;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0.0f;
	rasterizer_desc.DepthClipEnable = true;
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.FrontCounterClockwise = false;
	rasterizer_desc.ScissorEnable = false;

	dev->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
	devcon->RSSetState(rasterizer_state);

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

	devcon->IASetInputLayout(nullptr);
	devcon->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// run the message loop
	while (!EXIT_PROGRAM)
	{
		// do 3D rendering on the back buffer here
		
		// clear the back buffer to a deep blue
		devcon->OMSetRenderTargets(1, &backbuffer, nullptr);

		devcon->ClearRenderTargetView(backbuffer, clear_color);
		// Render the triangle.
		devcon->Draw(6, 0);

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
	ID3D11Texture2D* pBackBuffer;
	DX::ThrowIfFailed(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));

	// use the back buffer address to create the render target
	DX::ThrowIfFailed(dev->CreateRenderTargetView(pBackBuffer, nullptr, &backbuffer));
	pBackBuffer->Release();

	// set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, nullptr);

	D3D11_RASTERIZER_DESC rasterDesc;

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	DX::ThrowIfFailed(dev->CreateRasterizerState(&rasterDesc, &raster_state));
	
	// Now set the rasterizer state.
	devcon->RSSetState(raster_state);


	D3D11_TEXTURE2D_DESC depthBufferDesc;

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
	DX::ThrowIfFailed(dev->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer));


	// Set the viewport
	D3D11_VIEWPORT viewport = {};

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = WIDTH;
	viewport.Height = HEIGHT;

	devcon->RSSetViewports(1, &viewport);

	return true;
}

HRESULT compile_shader(
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

void cleanup()
{
	swapchain->Release();
	dev->Release();
	devcon->Release();
	backbuffer->Release();
}
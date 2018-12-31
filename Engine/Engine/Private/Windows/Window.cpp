#include "Window.h"

#include "Renderer.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace
{
	constexpr LPCWSTR wndClassName = L"EngineMainWindow";
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

namespace App
{
    class Window::NativeWindow
    {
    public:
        NativeWindow(const std::string& title, const uint16_t width, const uint16_t height)
        {
			WNDCLASSEXW wcex;

			ZeroMemory(&wcex, sizeof(wcex));
			wcex.cbSize = sizeof(wcex);
			wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wcex.lpfnWndProc = (WNDPROC)WndProc;
			wcex.hInstance = GetModuleHandleW(NULL);
			wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
			wcex.lpszClassName = wndClassName;

			RegisterClassExW(&wcex);

			CREATESTRUCTW cs;
			ZeroMemory(&cs, sizeof(cs));

			cs.x = 0;
			cs.y = 0;
			cs.cx = width;
			cs.cy = height;
			cs.hInstance = wcex.hInstance;
			cs.lpszClass = wcex.lpszClassName;
			cs.lpszName = wndClassName;
			cs.style = WS_OVERLAPPEDWINDOW;

			mNativeHandle = ::CreateWindowExW(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

			::ShowWindow(mNativeHandle, SW_SHOWDEFAULT);
			::UpdateWindow(mNativeHandle);
        }

		void NativeWindow::Update()
		{
			MSG msg;

			while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					// NOTE: While GLFW does not itself post WM_QUIT, other processes
					//       may post it to this one, for example Task Manager
					// HACK: Treat WM_QUIT as a close on all windows
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
				}
			}
		}
        
        ~NativeWindow()
        {
			DestroyWindow(mNativeHandle);
			::UnregisterClassW(wndClassName, GetModuleHandleW(nullptr));
        }
        
    public:
		HWND mNativeHandle{ nullptr };
    };
    
    Window::Window(const std::string& title, const uint16_t width, const uint16_t height)
        : mTitle(title)
        , mWidth(width)
        , mHeight(height)
        , mNativeWindow(std::make_unique<App::Window::NativeWindow>(title, width, height))
	{	
		Renderer::RendererServiceLocator::Service()->CreateWindowSurface(mSurface, (void*)mNativeWindow->mNativeHandle);
		//renderAPI->CreateSwapChain(mSwapChain, mSurface, mWidth, mHeight);
    }

	void Window::Update()
	{
		mNativeWindow->Update();
	}
    
    Window::~Window()
    {
    }
}

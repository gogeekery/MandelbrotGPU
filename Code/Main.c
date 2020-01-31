#define _CRT_SECURE_NO_WARNINGS
//#define GLEW_STATIC

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")

#ifdef GLEW_STATIC
	#pragma comment(lib, "glew32s.lib")
#else
	#pragma comment(lib, "glew32.lib")
#endif


#ifndef _DEBUG

	#define _SECURE_SCL 0
	#define _HAS_EXCEPTIONS 0
	#define _HAS_ITERATOR_DEBUGGING 0

	#pragma optimize("gsy", on)

#endif


#include <Windows.h>
#include <stdio.h>

#include <gl/glew.h>
#include <gl/GL.h>


#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


#define WINTITLE			"Mandelbrot"
#define WINCLASS			WINTITLE



// -------- Globals ----------------

HDC gWndDc;

// OpenGL viewport
int gScreenX, gScreenY, gScreenW, gScreenH;

// View X/Y/Scale for exploring the Mandelbrot -- updated in WM_MOUSEMOVE
float gViewX = 0.7F, gViewY = 0.0F, gViewS = 2.2F;

// The fragment shader colors each pixel on the quad to render a 2D mandelbrot
GLuint gShaderProgram;



// ---------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	static HGLRC hRC;
	static int lMB;

	switch (Msg) {

		case WM_CREATE:
		{

			PIXELFORMATDESCRIPTOR PFD = {			// Set the pixel format for the device context (HDC)

				sizeof(PIXELFORMATDESCRIPTOR),								// Size of structure
				1,															// Default version
				PFD_DRAW_TO_WINDOW |										// Window drawing support
				PFD_SUPPORT_OPENGL |										// OpenGL support
				PFD_DOUBLEBUFFER,											// Double buffering support
				PFD_TYPE_RGBA,												// RGBA color mode
				24,															// BPP color mode
				0, 0, 0, 0, 0, 0,											// Ignore color bits
				0,															// No alpha buffer
				0,															// Ignore shift bit
				0,															// No accumulation buffer
				0, 0, 0, 0,													// Ignore accumulation bits
				0,															// Z-buffer size
				0,															// No stencil buffer
				0,															// No aux buffer
				PFD_MAIN_PLANE,												// Main drawing plane
				0,															// Reserved
				0, 0, 0														// Layer masks ignored

			};

			gWndDc = GetDC(hWnd);

			SetPixelFormat(gWndDc, ChoosePixelFormat(gWndDc, &PFD), &PFD);

			hRC = wglCreateContext(gWndDc);
			wglMakeCurrent(gWndDc, hRC);

		}
		break;

		case WM_DESTROY:

			wglMakeCurrent(gWndDc, NULL);
			wglDeleteContext(hRC);

			ShowCursor(TRUE);

			PostQuitMessage(0);

		break;

		case WM_SYSCOMMAND:

			switch (wParam) {

				case SC_SCREENSAVE:								// Screensaver trying to start
				case SC_MONITORPOWER:							// Monitor going to powersave mode
					return 0;									// Return 0 (prevent it from happening)

				default:										// Other SYSCOMMAND; Do nothing
				break;

			}

		break;

		case WM_RBUTTONDOWN:
			lMB = 2;
		break;

		case WM_LBUTTONDOWN:
			lMB = 1;
		break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			lMB = 0;
		break;

		case WM_MOUSEMOVE:
		{

			float lNXP = ((float)LOWORD(lParam) / (float)gScreenW - 0.5F);
			float lNYP = ((float)HIWORD(lParam) / (float)gScreenH - 0.5F);
			static float lOXP, lOYP;

			if (lMB == 1) {

				gViewX += (lNXP - lOXP) * gViewS;
				gViewY -= (lNYP - lOYP) * gViewS;

				glUniform2f(glGetUniformLocation(gShaderProgram, "cntr"), gViewX, gViewY);

			} else if(lMB == 2) {

				gViewS *= 1.0F+(lNYP-lOYP);
				glUniform1f(glGetUniformLocation(gShaderProgram, "scal"), gViewS);

			}

			lOXP = lNXP;
			lOYP = lNYP;

		}
		break;

		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);

	}

	return 0;

}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR CmdLineV, int CmdLineC) {

	WNDCLASSEX	wClass;											// The Window class structure
	HWND		hWnd;

	wClass.cbSize			= sizeof(WNDCLASSEX);
	wClass.style			= CS_HREDRAW | CS_VREDRAW;
	wClass.lpfnWndProc		= WndProc;
	wClass.cbClsExtra		= 0;
	wClass.cbWndExtra		= 0;
	wClass.hInstance		= hInstance;
	wClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wClass.hbrBackground	= NULL;
	wClass.lpszMenuName		= NULL;
	wClass.lpszClassName	= WINCLASS;
	wClass.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wClass);									// Finally, Register the Class!

	// Enable strict memory access error checking
	#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF |_CRTDBG_CHECK_CRT_DF);
		_CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	#endif


	{	// Prepare for fullscreen!
		RECT wRect;

		GetWindowRect(GetDesktopWindow(), &wRect);
		AdjustWindowRectEx(&wRect, WS_POPUP, FALSE, WS_EX_APPWINDOW);

		// If fullscreen force a 16:9 or 1:1
		gScreenW = (int)(wRect.right-32);
		gScreenH = (int)(gScreenW*0.5625);//(int)wRect.bottom-64;
		gScreenX = (int)(wRect.right-gScreenW)/2;
		gScreenY = (int)(wRect.bottom-gScreenH)/2;
	}


	// Create the main Window:
	hWnd = CreateWindowEx(
		0,
		WINCLASS,												// Class name
		WINTITLE,												// App name
		WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,			// Style flags
		0, 0,													// X and Y coords
		gScreenW,												// Width
		gScreenH,												// Height
		NULL,													// Handle to parent
		NULL,													// Handle to menu
		hInstance,												// Application Inst
		NULL
	);


	ShowWindow(hWnd, SW_SHOW);									// Display window
	UpdateWindow(hWnd);


	{	// Force full screen
		WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };
		DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
		MONITORINFO mi = { sizeof(mi) };

		if (GetWindowPlacement(hWnd, &g_wpPrev) &&
			GetMonitorInfo(MonitorFromWindow(hWnd,MONITOR_DEFAULTTOPRIMARY), &mi)) {

			SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(
				hWnd, HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right  - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED
			);

		}
	}	// ----------


	{	// Initialize GLEW and start fragment shader program

		// Defines "glfShaderCode" with the MandelBrot GLSL fragment shader code to compile.
		#include "glfMB.h"

		GLint glfShaderObj;


		{	// Initialize GLEW

			glewInit();

			if (!glewIsSupported("GL_VERSION_2_0")) {
				MessageBox(hWnd, "OpenGL 2.0 unsupported.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
				return 1;
			}

		}

		// Create a viewport using the 16:9 rounded fullscreen ratio.
		glViewport(gScreenX, gScreenY, gScreenW, gScreenH);

		glfShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(glfShaderObj, 1, &glfShaderCode, NULL);
		glCompileShader(glfShaderObj);

		{	// Compile shader and check for errors

			GLint lSuccess;
			char lLog[1024];
			glGetShaderiv(glfShaderObj, GL_COMPILE_STATUS, &lSuccess);

			if (!lSuccess) {
				glGetShaderInfoLog(glfShaderObj, 1024, NULL, lLog);
				MessageBox(hWnd, lLog, "SHADER COMPILATION FAILED", MB_OK|MB_ICONEXCLAMATION);
				return 1;
			}

		}

		// ---
		gShaderProgram = glCreateProgram();
		glAttachShader(gShaderProgram, glfShaderObj);
		glLinkProgram(gShaderProgram);

		glUseProgram(gShaderProgram);

		glUniform2f(glGetUniformLocation(gShaderProgram, "cntr"), gViewX, gViewY);
		glUniform1f(glGetUniformLocation(gShaderProgram, "scal"), gViewS);

	}

	// -----------------------

	for (;;) {

		MSG lMsg;
		const DWORD lStTime = timeGetTime();

		// Hold ESC to abort
		if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) > 0) {
			DestroyWindow(hWnd);
		}

		while (PeekMessage(&lMsg, NULL, 0, 0, PM_REMOVE)) {

			if (lMsg.message == WM_QUIT)
				return (int)lMsg.wParam;

			TranslateMessage(&lMsg);
			DispatchMessage(&lMsg);

		}

		// Render the QUAD
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(-1, -1);
			glTexCoord2f(1, 0);
			glVertex2f(1, -1);
			glTexCoord2f(1, 1);
			glVertex2f(1, 1);
			glTexCoord2f(0, 1);
			glVertex2f(-1, 1);
		glEnd();
		// -------------------

		SwapBuffers(gWndDc);


		// Reduce CPU usage..
		while ((timeGetTime()-lStTime) < 60)
			Sleep(1);

	}

	return 0;

}


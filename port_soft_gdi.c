#include <Windows.h>

#include <stdio.h>

#include "ref_soft/r_local.h"
#include "client/keys.h"

#include "quake2.h"

static int _width = 640;
static int _height = 480;
static int _creatingWindow = 0;
static HWND _Hwnd = 0;
static HDC _Hdc = 0;
static BITMAPINFO* _bitmapInfo = NULL;

static int _window_center_x = 0;
static int _window_center_y = 0;
static int _mouseCaptured = 0;
int _old_mouse_buttonstate = 0;

static const char _windowClassName[] = "Quake2WindowClass";
static const char _windowTitle[] = "Quake2";


static char* _videoBuffer = NULL;
static const unsigned char* _palette = NULL;


void HandleMouseButton(int mouse_buttonstate)
{
	for (int i = 0; i < 3; i++)
	{
		if ((mouse_buttonstate & (1 << i)) &&
			!(_old_mouse_buttonstate & (1 << i)))
		{
			Quake2_SendKey(K_MOUSE1 + i, true);
		}

		if (!(mouse_buttonstate & (1 << i)) &&
			(_old_mouse_buttonstate & (1 << i)))
		{
			Quake2_SendKey(K_MOUSE1 + i, false);
		}
	}

	_old_mouse_buttonstate = mouse_buttonstate;
}

static LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		if (_creatingWindow == 0)
		{
			PostQuitMessage(0);
			ExitProcess(0);
		}
		break;
	case WM_KEYDOWN:
		Quake2_SendKey(ConvertToQuakeKey(wParam), 1);
		break;
	case WM_KEYUP:
		Quake2_SendKey(ConvertToQuakeKey(wParam), 0);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
	{
		int	temp;

		temp = 0;

		if (wParam & MK_LBUTTON)
			temp |= 1;

		if (wParam & MK_RBUTTON)
			temp |= 2;

		if (wParam & MK_MBUTTON)
			temp |= 4;

		HandleMouseButton(temp);
	}
	break;
	default:
		return DefWindowProcA(hwnd, msg, wParam, lParam);
	}
	return 0;
}

static void RegisterWindow()
{
	WNDCLASSEXA wc;

	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.style = 0;
	wc.lpfnWndProc = MessageHandler;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = 0;
	wc.hIcon = 0;
	wc.hCursor = 0;
	wc.hbrBackground = 0;
	wc.lpszMenuName = 0;
	wc.lpszClassName = _windowClassName;
	wc.hIconSm = 0;

	if (!RegisterClassExA(&wc))
	{
		printf("Window Registration Failed!");

		exit(-1);
	}
}

static void CreateBitmapInfo()
{
	int bitmapInfoSize = sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD);
	_bitmapInfo = malloc(bitmapInfoSize);
	memset(_bitmapInfo, 0, bitmapInfoSize);
	_bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	_bitmapInfo->bmiHeader.biWidth = _width;
	_bitmapInfo->bmiHeader.biHeight = -_height;
	_bitmapInfo->bmiHeader.biPlanes = 1;
	_bitmapInfo->bmiHeader.biBitCount = 8;
}

static void SetupPalette()
{
	if (NULL == _palette)
	{
		return;
	}

	unsigned const char* p = _palette;

	for (int i = 0; i < 256; ++i)
	{
		_bitmapInfo->bmiColors[i].rgbRed = *p++;
		_bitmapInfo->bmiColors[i].rgbGreen = *p++;
		_bitmapInfo->bmiColors[i].rgbBlue = *p++;

		p++;
	}
}

static void SetupWindow()
{
	_creatingWindow = 1;
	if (_videoBuffer)
	{
		free(_videoBuffer);
		_videoBuffer = NULL;
	}
	_videoBuffer = malloc(_width * _height);

	vid.buffer = _videoBuffer;
	vid.rowbytes = _width;

	if (_Hwnd)
	{
		if (_Hdc)
		{
			ReleaseDC(_Hwnd, _Hdc);
			_Hdc = NULL;
		}

		DestroyWindow(_Hwnd);
		_Hwnd = NULL;
	}

	RECT rect;
	rect.left = rect.top = 0;
	rect.right = _width;
	rect.bottom = _height;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindowExA(0, _windowClassName, _windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, 0, 0, 0, 0);
	if (hwnd)
	{
		_Hwnd = hwnd;

		_Hdc = GetDC(hwnd);
		ShowWindow(hwnd, SW_SHOW);
	}
	else
	{
		printf("Window Creation Failed!");
	}

	_creatingWindow = 0;
}


rserr_t SWimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
	int width = 0;
	int height = 0;

	ri.Con_Printf( PRINT_ALL, "Initializing GDI display\n");

	ri.Con_Printf (PRINT_ALL, "...setting mode %d:", mode );

	if ( !ri.Vid_GetModeInfo( &width, &height, mode ) )
	{
		ri.Con_Printf( PRINT_ALL, " invalid mode\n" );
		return rserr_invalid_mode;
	}

	ri.Con_Printf( PRINT_ALL, " %d %d\n", width, height );


  _width = width;
  _height = height;

  *pwidth = width;
  *pheight = height;

  SetupWindow();	

	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow (width, height);

	return rserr_ok;
}

void SWimp_Shutdown( void )
{
	//SDL_Quit();
}


int SWimp_Init(void *hInstance, void *w)
{
	SetupWindow();

	return true;
}

static qboolean SWimp_InitGraphics( qboolean fullscreen )
{
	return rserr_ok;
}

void SWimp_SetPalette( const unsigned char *palette )
{
	_palette = palette;

	SetupPalette();
}

void SWimp_BeginFrame( float camera_seperation )
{
}


void SWimp_EndFrame (void)
{
	MSG msg;
	memset(&msg, 0, sizeof(msg));

	while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	
	_bitmapInfo->bmiHeader.biWidth = _width;
	_bitmapInfo->bmiHeader.biHeight = -_height;

	StretchDIBits(_Hdc, 0, 0, _width, _height, 0, 0, _width, _height, _videoBuffer, _bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}


void SWimp_AppActivate( qboolean active )
{

}


int ConvertToQuakeKey(unsigned int keysym)
{
  int key = keysym;
  
  switch(keysym)
  {
  case VK_OEM_3:		key = '~'; break;
  case VK_NUMPAD9:            key = K_KP_PGUP; break;
  case VK_PRIOR:        key = K_PGUP; break;
	  
  case VK_NUMPAD3:            key = K_KP_PGDN; break;
  case VK_NEXT:        key = K_PGDN; break;
	
  case VK_NUMPAD7:            key = K_KP_HOME; break;
  case VK_HOME:            key = K_HOME; break;
	
  case VK_NUMPAD1:            key = K_KP_END; break;
  case VK_END:            key = K_END; break;
	
  case VK_NUMPAD4:            key = K_KP_LEFTARROW; break;
  case VK_LEFT:            key = K_LEFTARROW; break;
	
  case VK_NUMPAD6:            key = K_KP_RIGHTARROW; break;
  case VK_RIGHT:        key = K_RIGHTARROW; break;
	
  case VK_NUMPAD2:            key = K_KP_DOWNARROW; break;
  case VK_DOWN:            key = K_DOWNARROW; break;
	
  case VK_NUMPAD8:            key = K_KP_UPARROW; break;
  case VK_UP:            key = K_UPARROW; break;
	
  case VK_ESCAPE:        key = K_ESCAPE; break;
	
  case VK_RETURN:        key = K_ENTER; break;
	
  case VK_TAB:            key = K_TAB; break;
	
  case VK_F1:            key = K_F1; break;
  case VK_F2:            key = K_F2; break;
  case VK_F3:            key = K_F3; break;
  case VK_F4:            key = K_F4; break;
  case VK_F5:            key = K_F5; break;
  case VK_F6:            key = K_F6; break;
  case VK_F7:            key = K_F7; break;
  case VK_F8:            key = K_F8; break;
  case VK_F9:            key = K_F9; break;
  case VK_F10:            key = K_F10; break;
  case VK_F11:            key = K_F11; break;
  case VK_F12:            key = K_F12; break;
	
  case VK_BACK:        key = K_BACKSPACE; break;
	
  case VK_DELETE:        key = K_DEL; break;
	
  case VK_PAUSE:        key = K_PAUSE; break;
	
  case VK_SHIFT:        key = K_SHIFT; break;
	
  case VK_CONTROL:        key = K_CTRL; break;
	
  case VK_MENU:            key = K_ALT; break;
	
  case VK_NUMPAD5:            key = K_KP_5; break;
	
  case VK_INSERT:        key = K_INS; break;
  case VK_NUMPAD0:            key = K_KP_INS; break;
	
  case VK_MULTIPLY:        key = '*'; break;
  case VK_ADD:        key = K_KP_PLUS; break;
  case VK_SUBTRACT:        key = K_KP_MINUS; break;
  case VK_DIVIDE:        key = K_KP_SLASH; break;

  default: // assuming that the other sdl keys are mapped to ascii
	if (keysym < 128)
	  key = keysym;
	break;
  }


  return key;        
}


void QG_GetMouseDiff(int* dx, int* dy)
{
	POINT current_mouse_pos;
	if (GetCursorPos(&current_mouse_pos))
	{
		if (_mouseCaptured)
		{
			*dx = current_mouse_pos.x - _window_center_x;
			*dy = current_mouse_pos.y - _window_center_y;

			SetCursorPos(_window_center_x, _window_center_y);
		}
	}
}

void QG_CaptureMouse(void)
{
	RECT window_rect;
	GetWindowRect(_Hwnd, &window_rect);

	_window_center_x = (window_rect.right + window_rect.left) / 2;
	_window_center_y = (window_rect.top + window_rect.bottom) / 2;

	SetCursorPos(_window_center_x, _window_center_y);
	SetCapture(_Hwnd);
	ClipCursor(&window_rect);
	while (ShowCursor(FALSE) >= 0);

	_mouseCaptured = 1;
}

void QG_ReleaseMouse(void)
{
	ClipCursor(NULL);
	ReleaseCapture();
	while (ShowCursor(TRUE) < 0);

	_mouseCaptured = 0;
}

int main(int argc, char **argv)
{
	int time, oldtime, newtime;

	RegisterWindow();
	CreateBitmapInfo();

	Quake2_Init(argc, argv);

	oldtime = Quake2_Milliseconds ();
	while (1)
	{
	  do {
		newtime = Quake2_Milliseconds ();
		time = newtime - oldtime;
	  } while (time < 1);

	  Quake2_Frame(time);
	  
	  oldtime = newtime;
	}

	return 0;
}
// cond_clk_lib.cpp : 定义 DLL 应用程序的导出函数。
//
#include <vector>
#include <iterator>

#include <Windows.h>

#include <d3d9.h>


#pragma comment(lib,"d3d9.lib")
#include <WinError.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)


UINT Width = 0, Height = 0;

IDirect3D9*            pD3DScreenCapture = NULL;
IDirect3DDevice9*    pd3dDevice = NULL;
IDirect3DSurface9*    pSurface = NULL;


DLL_EXPORT void reset(void) {
	Width = Height = 0;
	if (pSurface) {
		pSurface->Release();
		pSurface = NULL;
	}
	if (pd3dDevice) {
		pd3dDevice->Release();
		pd3dDevice = NULL;
	}
	if (pD3DScreenCapture) {
		pD3DScreenCapture->Release();
		pD3DScreenCapture = NULL;
	}
}

#pragma comment(linker,"/export:cleanup=reset")

bool initialize(void) {
	reset();
	HWND hWnd = NULL;// GetDesktopWindow();
	D3DDISPLAYMODE    ddm;
	D3DPRESENT_PARAMETERS    d3dpp;

	do {
		if ((pD3DScreenCapture = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
			break;
		if (FAILED(pD3DScreenCapture->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &ddm)))
			break;

		ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

		d3dpp.Windowed = TRUE;
		d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		d3dpp.BackBufferFormat = ddm.Format;
		d3dpp.BackBufferHeight = Height = ddm.Height;
		d3dpp.BackBufferWidth = Width = ddm.Width;
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		d3dpp.BackBufferCount = 1;
		d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
		d3dpp.hDeviceWindow = hWnd;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		d3dpp.FullScreen_RefreshRateInHz = /*ddm.RefreshRate*/D3DPRESENT_RATE_DEFAULT;

		HRESULT res;
		if (FAILED(res = pD3DScreenCapture->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice)))
			break;

		if (FAILED(pd3dDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL)))
			break;
		return true;
	} while (false);
	return false;
}


BOOL APIENTRY DllMain(HMODULE,
	DWORD,
	LPVOID
)
{
	return TRUE;
}


DLL_EXPORT UINT width(void) {
	if (NULL == pSurface)
		initialize();
	return Width;
}
DLL_EXPORT UINT height(void) {
	if (NULL == pSurface)
		initialize();
	return Height;
}

DLL_EXPORT ULONG capture(PVOID buffer, ULONG limit) {
	if (NULL == pSurface) {
		if (!initialize())
			return 0;
	}
	ULONG required_size = 4 * Width*Height;
	if (limit < required_size)
		return required_size;
	do {
		pd3dDevice->GetFrontBufferData(0, pSurface);
		D3DLOCKED_RECT    lockedRect;
		RECT area = { 0,0,(LONG)Width,(LONG)Height };
		if (FAILED(pSurface->LockRect(&lockedRect, &area, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY))) {
			required_size = 0;
			break;
		}

		if (Width * 4 != (UINT)lockedRect.Pitch) {
			required_size = 0;
			break;
		}

		RtlMoveMemory(buffer, lockedRect.pBits, required_size);


	} while (false);
	pSurface->UnlockRect();
	return required_size;
}

#pragma warning(disable : 4309)
template<typename Out>
bool translate(Out out, char c) {
	if (isprint(c) || c == '\t' || c == '\n' || c == 27)
		;
	else
		return false;

	bool shift = true;
	switch (c) {
	case '~':
		c = '`';
		break;
	case '!':
		c = '1';
		break;
	case '@':
		c = '2';
		break;
	case '#':
		c = '3';
		break;
	case '$':
		c = '4';
		break;
	case '%':
		c = '5';
		break;
	case '^':
		c = '6';
		break;
	case '&':
		c = '7';
		break;
	case '*':
		c = '8';
		break;
	case '(':
		c = '9';
		break;
	case ')':
		c = '0';
		break;
	case '_':
		c = '-';
		break;
	case '+':
		c = '=';
		break;
	case '<':
		c = ',';
		break;
	case '>':
		c = '.';
		break;
	case '?':
		c = '/';
		break;
	case ':':
		c = ';';
		break;
	case '\"':
		c = '\'';
		break;
	case '{':
		c = '[';
		break;
	case '}':
		c = ']';
		break;
	case '|':
		c = '\\';
		break;
	default:
		shift = false;
	}
	if (c >= 'A' && c <= 'Z')
		shift = true;
	if (c >= 'a' && c <= 'z')
		c -= 0x20;


	switch (c) {
	case '\'':
		c = 0xDE;
		break;
	case ',':
		c = 0xBC;
		break;
	case '-':
		c = 0xBD;
		break;
	case '.':
		c = 0xBE;
		break;
	case '/':
		c = 0xBF;
		break;
	case ';':
		c = 0xBA;
		break;
	case '=':
		c = 0xBB;	//???
		break;
	case '[':
		c = 0xDB;
		break;
	case '\\':
		c = 0xDC;
		break;
	case ']':
		c = 0xDD;
		break;
	case '`':
		c = 0xC0;
		break;
	case '\n':
		c = 0x0D;
		break;
	}

	INPUT cur = { 1,0 };

	if (shift) {
		cur.ki.wVk = VK_SHIFT;
		*out++ = cur;
	}

	cur.ki.wVk = c;
	*out++ = cur;
	cur.ki.dwFlags = KEYEVENTF_KEYUP;
	*out++ = cur;

	if (shift) {
		cur.ki.wVk = VK_SHIFT;
		*out++ = cur;
	}

	return true;
}

DLL_EXPORT UINT type(LPCSTR string) {
	std::vector<INPUT> inputs;
	UINT count = 0;
	while (*string) {
		if (!translate(back_inserter(inputs), *string++))
			break;
		++count;
	}
	if (inputs.size() != SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT)))
		return 0;
	return count;
}


DLL_EXPORT BOOL move(UINT x, UINT y) {
	if (NULL == pSurface) {
		if (!initialize())
			return FALSE;
	}
	INPUT input = { 0 };
	input.mi.dx = (LONG)(x * ((double)0xFFFF / Width));
	input.mi.dy = (LONG)(y * ((double)0xFFFF / Height));
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	return 1 == SendInput(1, &input, sizeof(INPUT)) ? TRUE : FALSE;

}



//DLL_EXPORT BOOL click(UINT x, UINT y,UINT button) {
DLL_EXPORT BOOL click(UINT button) {
	if (button > 2)
		return FALSE;

	static const DWORD down[] = { MOUSEEVENTF_LEFTDOWN,MOUSEEVENTF_RIGHTDOWN ,MOUSEEVENTF_MIDDLEDOWN };
	static const DWORD up[] = { MOUSEEVENTF_LEFTUP ,MOUSEEVENTF_RIGHTUP ,MOUSEEVENTF_MIDDLEUP };

	INPUT inputs[2] = { 0 };
	//TRICK : inputs[i].type==0 => INPUT_MOUSE
	inputs[0].mi.dwFlags = down[button];
	inputs[1].mi.dwFlags = up[button];
	return 2 == SendInput(2, inputs, sizeof(INPUT)) ? TRUE : FALSE;
	
}

DLL_EXPORT BOOL scroll(int step) {
	INPUT input = { 0 };
	input.mi.mouseData = (DWORD)(-step);
	input.mi.dwFlags = MOUSEEVENTF_WHEEL;
	return 1 == SendInput(1, &input, sizeof(INPUT)) ? TRUE : FALSE;
}

#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

int main() {

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
		
	WNDCLASS wndclass;
	CHAR* providerClass = "AsyncSelect";
	
	HWND window;
	wndclass.style = 0;
	wndclass.lpfnWndProc = WNDPROC WinProc
		wndclass.cbClsExtra = wndclass.cbWndExtra
		=
		wndclass.hInstance
		= NULL
		wndclass.hIcon
		= LoadIcon NULL, IDI_APPLICATION
		wndclass.hCursor
		= LoadCursor NULL, IDC_ARROW
		wndclass.hbrBackground
		= HBRUSH GetStockObject WHITE_BRUSH
		wndclass.lpszMenuName
		= NULL
		wndclass.lpszClassName
		= LPCWSTR providerClass
		if
			RegisterClass wndclass ) ==
			return NULL;
	// Create a window
	if
		((window = CreateWindow LPCWSTR providerClass, L""L"", WS_OVERLAPPEDWINDOW
			CW_USEDEFAULT
			, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT
			NULL
			, NULL, NULL, NULL)) == NULL
		return NULL
}
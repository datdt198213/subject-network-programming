// Client_App.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Client_App.h"
#include "winsock2.h"
#include "string"
#include "stdio.h"

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

#define IDC_LIST_MESSAGE    19
#define IDC_LIST_CLIENT     20
#define IDC_BUTTON_SEND     2121
#define IDC_EDIT_MESSAGE    2122
#define VM_USER             23
#define WM_SOCKET WM_USER + 1
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

SOCKADDR_IN addr;
SOCKET client;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    // khoi tao phien ban
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // tao client ket noi den server
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // tao dia chi
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8000);

    int success = connect(client, (sockaddr*)&addr, sizeof(addr));
    
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENTAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTAPP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENTAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIENTAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_ACCEPT| FD_READ);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
       10, 10, 550, 40, hWnd, (HMENU)IDC_EDIT_MESSAGE, GetModuleHandle(NULL), NULL);
   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Send"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
       10, 60, 550, 40, hWnd, (HMENU)IDC_BUTTON_SEND, GetModuleHandle(NULL), NULL);
   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
       10, 110, 400, 350, hWnd, (HMENU)IDC_LIST_MESSAGE, GetModuleHandle(NULL), NULL);
   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
       420, 110, 150, 350, hWnd, (HMENU)IDC_LIST_CLIENT, GetModuleHandle(NULL), NULL);
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SOCKET:
    {
        if (WSAGETSELECTERROR(lParam)) {
            closesocket(wParam);
        }
        else
        {
            if (WSAGETSELECTEVENT(lParam) == FD_READ)
            {
                char buf[256], cmd[50], msgRecv[256];
                int ret = recv(wParam, buf, sizeof(buf), 0);
                buf[ret] = 0;
                
                memset(cmd, 0, sizeof(cmd));
                sscanf(buf, "%s %s", cmd, msgRecv);

                if (strcmp(cmd, "CONNECT_OK") == 0)
                {
                    MessageBox(hWnd, (LPCWSTR)L"Dang nhap thanh cong", (LPCWSTR)L"Thong bao", MB_ICONINFORMATION | MB_OK);
                    // XOA CHU HIEN TAI CO TRONG TEXT
                } 
                else if (strcmp(cmd, "ERROR") == 0)
                {
                    MessageBox(hWnd, (LPCWSTR)L"ID cua ban nhap loi", (LPCWSTR)L"Canh bao dang nhap", MB_ICONWARNING | MB_OK);
                }
                else if (strcmp(cmd, "CONNECT_ERROR") == 0)
                {
                    MessageBox(hWnd, (LPCWSTR)L"ID da duoc su dung. Hay chon ID khac", (LPCWSTR)L"Canh bao trung dang nhap", MB_ICONWARNING | MB_OK);
                }
                else
                {
                    // DANG NHAP THAT BAI
                }
            }
            else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
            {
                closesocket((SOCKET)wParam);
            }
        }
    }
    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDC_BUTTON_SEND:
            {
                char buf[256];
                GetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, buf, sizeof(buf));
                send(client, buf, strlen(buf), 0);
            }
            break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
